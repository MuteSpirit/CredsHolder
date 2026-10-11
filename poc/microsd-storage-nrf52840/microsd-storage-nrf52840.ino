#include <cstring>
#include "VeraCryptDisk.h"
#include "veracrypt/Volumes.h"
#include "wolfssl/wolfcrypt/pwdbased.h"
#include "wolfssl/wolfcrypt/aes.h"

// Static members
uint8_t  VeraCryptDisk::s_primary_key[32];
uint8_t  VeraCryptDisk::s_secondary_key[32];
uint64_t VeraCryptDisk::s_data_start;
IO*      VeraCryptDisk::s_io = nullptr;

// Persistent XTS context (used for data area encryption/decryption)
static XtsAes g_xts_ctx;
static bool   g_xts_initialized = false;

bool VeraCryptDisk::init(IO* io,
                         const char* password,
                         uint32_t pim,
                         PrfType prf,
                         LBA_t header_sector)
{
    s_io = io;

    // ========== 1. Read the volume header ==========
    uint8_t header_raw[TC_VOLUME_HEADER_SIZE];
    constexpr UINT SECTORS_TO_READ = TC_VOLUME_HEADER_SIZE / 512;

    if (!s_io->read(header_raw, header_sector, SECTORS_TO_READ)) {
        return false;
    }

    const uint8_t* salt = header_raw;
    constexpr size_t SALT_SIZE = 64;

    // ========== 2. Compute PBKDF2 iteration count ==========
    uint32_t iterations;
    if (prf == PrfType::RIPEMD160) {
        iterations = (pim == 0) ? 655331 : (pim * 2048);
    } else {
        iterations = (pim == 0) ? 500000 : (15000 + pim * 1000);
    }

    // ========== 3. Derive the header key with wolfSSL PBKDF2 ==========
    int hashType;
    switch (prf) {
        case PrfType::SHA512:    hashType = WC_SHA512;    break;
        case PrfType::SHA256:    hashType = WC_SHA256;    break;
        case PrfType::RIPEMD160: return false;  // wc_PBKDF2 does not support RIPEMD-160
        default: return false;
    }

    uint8_t derived_key[64];
    int ret = wc_PBKDF2(derived_key,
                        (const byte*)password, strlen(password),
                        salt, SALT_SIZE,
                        iterations,
                        sizeof(derived_key),
                        hashType);
    if (ret != 0) return false;

    // ========== 4. Decrypt the header with wolfSSL XTS ==========
    // VeraCrypt header: 448 bytes, data unit index = 0
    uint8_t header_decrypted[448];
    memcpy(header_decrypted, header_raw + 64, 448);

    XtsAes header_xts;
    ret = wc_AesXtsSetKey(&header_xts,
                          derived_key,       // K1 || K2 (64 bytes)
                          sizeof(derived_key),
                          AES_DECRYPTION,
                          NULL, INVALID_DEVID);
    if (ret != 0) return false;

    ret = wc_AesXtsDecryptSector(&header_xts,
                                 header_decrypted,   // out
                                 header_decrypted,   // in (in-place decryption)
                                 448,                // sz
                                 0);                 // sector = 0
    wc_AesXtsFree(&header_xts);

    if (ret != 0) return false;

    // ========== 5. Verify the magic string ==========
    if (memcmp(header_decrypted, "VERA", 4) != 0) return false;

    // ========== 6. Verify the CRC-32 checksums ==========
    uint32_t stored_crc1, stored_crc2;
    memcpy(&stored_crc1, header_decrypted + 8, 4);
    memcpy(&stored_crc2, header_decrypted + 188, 4);
    stored_crc1 = __builtin_bswap32(stored_crc1);
    stored_crc2 = __builtin_bswap32(stored_crc2);

    uint32_t calc_crc1 = crc32(header_decrypted + 192, 448 - 192);
    uint32_t calc_crc2 = crc32(header_decrypted, 188);

    if (stored_crc1 != calc_crc1 || stored_crc2 != calc_crc2) return false;

    // ========== 7. Extract the master keys ==========
    memcpy(s_primary_key,   header_decrypted + 192, 32);
    memcpy(s_secondary_key, header_decrypted + 224, 32);

    // ========== 8. Get the data area start position ==========
    uint64_t data_start_bytes;
    memcpy(&data_start_bytes, header_decrypted + 44, 8);
    data_start_bytes = __builtin_bswap64(data_start_bytes);
    s_data_start = header_sector + (data_start_bytes / 512);

    // ========== 9. Initialize the data area XTS context ==========
    // Concatenate K1 || K2 as the XTS key
    uint8_t data_xts_key[64];
    memcpy(data_xts_key,      s_primary_key,   32);
    memcpy(data_xts_key + 32, s_secondary_key, 32);

    ret = wc_AesXtsSetKey(&g_xts_ctx,
                          data_xts_key,
                          sizeof(data_xts_key),
                          AES_DECRYPTION,  // Read direction; reset to ENCRYPTION when writing
                          NULL, INVALID_DEVID);
    if (ret != 0) return false;

    g_xts_initialized = true;
    return true;
}

// Read a single sector (decrypt)
bool VeraCryptDisk::read_sector(uint8_t* buf, uint64_t lba) {
    if (!g_xts_initialized) return false;

    uint64_t encrypted_offset = (lba * 512 - s_data_start * 512) / 512;
    // Or per the spec: tweak = encrypted_area_start / 512 + lba

    // Read the encrypted data from IO
    if (!s_io->read(buf, s_data_start + lba, 1)) return false;

    // wolfSSL XTS decryption; the sector parameter is the data unit index
    int ret = wc_AesXtsDecryptSector(&g_xts_ctx, buf, buf, 512,
                                     s_data_start + lba);
    return ret == 0;
}
