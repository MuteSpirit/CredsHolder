#ifndef __ENCRYPTED_BLOCK_STORAGE__
#define __ENCRYPTED_BLOCK_STORAGE__

#include <inttypes.h>
#include <stdio.h>
#include <AES.h>

#include "block_storage.hpp"

// TODO: how handle power cut? add "flush()" method into BlockStorage?

#define AES_BLOCK_SIZE 16

/// @brief Perform encryption for all data on underline block storage
/// Pay attention that this class interpret 16 bytes block filled
/// by zeroes as formatted empty blocks. Please make wiping underline
/// block storage with zeroes before give it for EncryptedBlockStorage
/// management
class EncryptedBlockStorage : public BlockStorage
{
public:
    EncryptedBlockStorage(BlockStorage &bs);
    ~EncryptedBlockStorage();

public:
    /// @brief Set secret key for AES stuff. Storage cannot work without that.
    ///
    /// Without "init" call the read/write operations will be ignored
    /// AES-256 requires a key that is exactly 32 bytes (256 bits).
    /// @return false if the key length is not supported, or the key is somehow "weak" and unusable by this cipher.
    bool init(const uint8_t *key, const size_t keySize);

    virtual size_t minAddr() const override;
    virtual size_t maxAddr() const override;

    virtual bool isAddrOk(const size_t addr) const override;

    virtual uint8_t read(const size_t addr) override;
    virtual void write(const size_t addr, const uint8_t b) override;

    virtual void read(const size_t addr, uint8_t *buf, const size_t size) override;
    virtual void write(const size_t addr, const uint8_t *buf, const size_t size) override;

    virtual void factoryReset() override;

protected:
    BlockStorage &bs_; /// underline not encrypted block storage

    AES256 aes_; /// for encrypt/decrypt "bs_" content
    bool aesKeySet_ {false}; /// flag that "init(...)" success call happen

    uint32_t blockStartAddr_ {0}; /// start address on "bs_" of block which has been cached in "decryptedBlock_"
    uint8_t decryptedBlock_[AES_BLOCK_SIZE]; /// cached block with decrypted content
    int8_t decryptIdx_{-1};   /// decryptedBlock_ current position. -1 means that decryption did not happen yet
    bool cachedBlockIsDirty_ {false}; /// raise if content of decryptedBlock_ become dirty - modified but not flushed onto underline block storage
};

#endif // !__ENCRYPTED_BLOCK_STORAGE__
