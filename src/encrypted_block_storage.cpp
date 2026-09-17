#include "encrypted_block_storage.hpp"

#include <string.h>


EncryptedBlockStorage::EncryptedBlockStorage(BlockStorage &bs)
    : bs_(bs)
    , blockStartAddr_(bs_.minAddr())
{
}

EncryptedBlockStorage::~EncryptedBlockStorage()
{
    aes_.clear();
}

bool
EncryptedBlockStorage::init(const uint8_t *key, const size_t keySize)
{
    if (aes_.setKey(key, keySize)) {
        aesKeySet_ = true;
        return true;
    }
    return false;
}

size_t
EncryptedBlockStorage::minAddr() const
{
    return bs_.minAddr();
}

size_t
EncryptedBlockStorage::maxAddr() const
{
    return bs_.maxAddr();
}

bool
EncryptedBlockStorage::isAddrOk(const size_t addr) const
{
    return bs_.isAddrOk(addr);
}

uint8_t
EncryptedBlockStorage::read(const size_t addr)
{
    if (!aesKeySet_) {
        return 0;
    }

    if (addr < blockStartAddr_ || blockStartAddr_ + AES_BLOCK_SIZE <= addr) {
        // read outside cached block should happen
        // so it's needed to flash cached block into underline block storage
        // before start work with another one

        if (cachedBlockIsDirty_) {
            uint8_t block[AES_BLOCK_SIZE] = {0};
            aes_.encryptBlock(block, decryptedBlock_);
            bs_.write(blockStartAddr_, block, AES_BLOCK_SIZE);

            cachedBlockIsDirty_ = false;
        }

        decryptIdx_ = -1;
    }

    if (decryptIdx_ < 0) { // no decryption before
        // we should perform another decryption
        blockStartAddr_ = (addr / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;

        uint8_t block[AES_BLOCK_SIZE] = {0};
        bs_.read(blockStartAddr_, block, AES_BLOCK_SIZE);
        //
        // If we cannot say is block encrypted or plain on underline block storage
        // then it'll have a mess instead of data after "decryption" plain data
        // So assume that block filled by zeros is plain.
        bool wiped = true;
        for (size_t i = 0; i < AES_BLOCK_SIZE; ++i) {
            if (block[i]) {
                wiped = false;
                break;
            }
        }
        if (!wiped) {
            aes_.decryptBlock(decryptedBlock_, block);
        } else {
            memcpy(decryptedBlock_, block, AES_BLOCK_SIZE);
        }
    }

    decryptIdx_ = addr - blockStartAddr_;

    return decryptedBlock_[decryptIdx_];
}

void
EncryptedBlockStorage::write(const size_t addr, const uint8_t b)
{
    if (!aesKeySet_) {
        return;
    }

    if (addr < blockStartAddr_ || blockStartAddr_ + AES_BLOCK_SIZE <= addr) {
        // write outside cached block should happen
        // so it's needed to flash cached block into underline block storage
        // before start work with another one

        if (cachedBlockIsDirty_) {
            uint8_t block[AES_BLOCK_SIZE] = {0};
            aes_.encryptBlock(block, decryptedBlock_);
            bs_.write(blockStartAddr_, block, AES_BLOCK_SIZE);

            cachedBlockIsDirty_ = false;
        }
        decryptIdx_ = -1;
    }

    if (decryptIdx_ < 0) { // no decryption before
        blockStartAddr_ = (addr / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;

        // fill "decryptedBlock_" with original data located at underline data block
        uint8_t block[AES_BLOCK_SIZE] = {0};
        bs_.read(blockStartAddr_, block, AES_BLOCK_SIZE);
        //
        // If we cannot say is block encrypted or plain on underline block storage
        // then it'll have a mess instead of data after "decryption" plain data
        // So assume that block filled by zeros is plain.
        bool wiped = true;
        for (size_t i = 0; i < AES_BLOCK_SIZE; ++i) {
            if (block[i]) {
                wiped = false;
                break;
            }
        }
        if (!wiped) {
            aes_.decryptBlock(decryptedBlock_, block);
        } else {
            memcpy(decryptedBlock_, block, AES_BLOCK_SIZE);
        }
    }

    decryptIdx_ = addr - blockStartAddr_;
    decryptedBlock_[decryptIdx_] = b;
    cachedBlockIsDirty_ = true;
}

void
EncryptedBlockStorage::read(const size_t addr, uint8_t *buf, const size_t size)
{
    if (!aesKeySet_) {
        return;
    }

    for (size_t i = 0; i < size; ++i) {
        buf[i] = read(addr + i);
    }
}

void
EncryptedBlockStorage::write(const size_t addr, const uint8_t *buf, const size_t size)
{
    if (!aesKeySet_) {
        return;
    }

    for (size_t i = 0; i < size; ++i) {
        write(addr + i, buf[i]);
    }
}

void
EncryptedBlockStorage::factoryReset()
{
    bs_.factoryReset();
}
