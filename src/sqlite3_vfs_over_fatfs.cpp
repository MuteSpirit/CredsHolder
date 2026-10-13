// CredsHolder (Hardware Credential Manager)
// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Current file sources are based on 
// SQLite3 VFS example
//   https://sqlite.org/src/doc/trunk/src/test_onefile.c
//     with license:
//   2007 September 14
//
//   The author disclaims copyright to this source code.  In place of
//   a legal notice, here is a blessing:
//
//      May you do good and not evil.
//      May you find forgiveness for yourself and forgive others.
//      May you share freely, never taking more than you give.

// Current file sources are based also on
//   https://github.com/siara-cc/esp32_arduino_sqlite3_lib/blob/master/src/esp32.cpp
//     with license:
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//  
//       http://www.apache.org/licenses/LICENSE-2.0
//  
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

/// @file sqlite3_vfs_over_fatfs.cpp
///
/// @brief Define new SQLite3 Virtual File System (VFS) to allow SQLite3 work above 
/// TinyFATFS/FatFs.
///
/// @details Defines functions sqlite3_os_init and sqlite3_os_end
/// sqlite3_os_init transparently register new VFS as default VFS.
/// Memory alloc/free performs via sqlite3_malloc/sqlite3_free.
/// Let's 1st implementation will have no cache at VFS level because this 
/// abstraction is not aware about details of underline storage, which e.g. maybe
/// at RAM and we will have cache in RAM for storage in RAM.
#include "creds_holder.hpp"
#include "sqlite3_vfs_over_fatfs.hpp"

#include <string.h>

#include <Embedded_Template_Library.h>
#include <etl/string.h>
#include <etl/map.h>
#include <etl/algorithm.h>

#include "sqlite.hpp"


// !!! I had to change TinyFATFS:
// 1. ffconf.h: enable FF_MULTI_PARTITION
// 2. ffconf.h: enable FF_USE_LABEL
#if defined(USE_TINYUSB)
// Avoid including "driver/TinyUsbMscIO.h" because it causes target build error
// (at least for nRF52840 Pro Micro)
#  undef USE_TINYUSB
#    include <fatfs.h>
#  define USE_TINYUSB
#else
# include <fatfs.h>
#endif

// FIXME: remove suppress compiler warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"


namespace fatfs {
namespace vfs {

/// Name used to identify this VFS.
#define FS_VFS_NAME "fatfs"

#define FS_VFS_MAX_MOUNTS 32

/// Handle operations with opened file using FatFs interface
typedef struct fatfs_file {
  sqlite3_file base;
  FatFs* fatFs;
  FIL fl;   /// file on that volume
} fatfs_file_t;


/// Context for "fatfs" VFS
/// Will contain list of named mounted FatFs volumes
/// SQLite3 DB open function will accept filename like
///   "/<fatfs-volume-name>/<file-path>", e.g. "/ram0/test0.txt", "/ram1/t.txt", "/sd0/t2.list"
///   where <fatfs-volume-name> will mean concrete FatFs with concrete attached IO driver
class FatFsVfsCtx
{
public:
    using MountPrefix = etl::string<255>;
    struct MountPoint
    {
        FatFs* fatFs;
        FATFS fs;
    };

    etl::map<MountPrefix, MountPoint, FS_VFS_MAX_MOUNTS> mounts_; 
};

static FatFsVfsCtx defaultVfsCtx;

/*
** Method declarations for fs_file.
*/
static int fsClose(sqlite3_file*);
static int fsRead(sqlite3_file*, void*, int iAmt, sqlite3_int64 iOfst);
static int fsWrite(sqlite3_file*, const void*, int iAmt, sqlite3_int64 iOfst);
static int fsTruncate(sqlite3_file*, sqlite3_int64 size);
static int fsSync(sqlite3_file*, int flags);
static int fsFileSize(sqlite3_file*, sqlite3_int64 *pSize);
static int fsLock(sqlite3_file*, int);
static int fsUnlock(sqlite3_file*, int);
static int fsCheckReservedLock(sqlite3_file*, int *pResOut);
static int fsFileControl(sqlite3_file*, int op, void *pArg);
static int fsSectorSize(sqlite3_file*);
static int fsDeviceCharacteristics(sqlite3_file*);

/*
** Method declarations for fs_vfs.
*/
static int fsOpen(sqlite3_vfs*, const char *, sqlite3_file*, int , int *);
static int fsDelete(sqlite3_vfs*, const char *zName, int syncDir);
static int fsAccess(sqlite3_vfs*, const char *zName, int flags, int *);
static int fsFullPathname(sqlite3_vfs*, const char *zName, int nOut, char *zOut);
static void *fsDlOpen(sqlite3_vfs*, const char *zFilename);
static void fsDlError(sqlite3_vfs*, int nByte, char *zErrMsg);
static void (*fsDlSym(sqlite3_vfs*,void*, const char *zSymbol))(void);
static void fsDlClose(sqlite3_vfs*, void*);
static int fsRandomness(sqlite3_vfs*, int nByte, char *zOut);
static int fsSleep(sqlite3_vfs*, int microseconds);
static int fsCurrentTime(sqlite3_vfs*, double*);

static sqlite3_vfs default_fatfs_vfs = {
    .iVersion = 1,
    .szOsFile = sizeof(fatfs_file_t),
#if defined(FF_USE_LFN)
    .mxPathname = FF_LFN_BUF,
#else
    .mxPathname = FF_SFN_BUF,
#endif
    .pNext = 0,
    .zName = FS_VFS_NAME,
    .pAppData = &defaultVfsCtx, // VFS private context
    .xOpen = fsOpen,
    .xDelete = fsDelete,
    .xAccess = fsAccess,
    .xFullPathname = fsFullPathname,
    .xDlOpen = fsDlOpen,
    .xDlError = fsDlError,
    .xDlSym = fsDlSym,
    .xDlClose = fsDlClose,
    .xRandomness = fsRandomness,
    .xSleep = fsSleep,
    .xCurrentTime = fsCurrentTime,
    .xGetLastError = 0,
    .xCurrentTimeInt64 = 0,
    .xSetSystemCall = 0,
    .xGetSystemCall = 0,
    .xNextSystemCall = 0
};

static sqlite3_io_methods fs_io_methods = {
  1,                            /* iVersion */
  fsClose,                      /* xClose */
  fsRead,                       /* xRead */
  fsWrite,                      /* xWrite */
  fsTruncate,                   /* xTruncate */
  fsSync,                       /* xSync */
  fsFileSize,                   /* xFileSize */
  fsLock,                       /* xLock */
  fsUnlock,                     /* xUnlock */
  fsCheckReservedLock,          /* xCheckReservedLock */
  fsFileControl,                /* xFileControl */
  fsSectorSize,                 /* xSectorSize */
  fsDeviceCharacteristics,      /* xDeviceCharacteristics */
  0,                            /* xShmMap */
  0,                            /* xShmLock */
  0,                            /* xShmBarrier */
  0,                            /* xShmUnmap */
  0,                            /* xFetch */
  0                             /* xUnfetch */
};

////////////////////////////////////////////////////////////////////////////////
const char *name()
{
    return FS_VFS_NAME;
}

FatFsVfsCtx*
vfsCtx()
{
    return &defaultVfsCtx;
}

int 
register_mount(FatFsVfsCtx* ctx, FatFs *fatFs, const char* prefix)
{
    FRESULT rc = FR_OK;

    if (nullptr == fatFs || nullptr == prefix) {
        return (int)FR_INT_ERR;
    }

    if (nullptr == ctx) {
        ctx = &defaultVfsCtx;
    }

    auto emplRes = ctx->mounts_.try_emplace(prefix);
    if (!emplRes.second) { // could not register new mount point
        return (int)FR_INVALID_NAME;
    }

    typename FatFsVfsCtx::MountPoint& mp = (*emplRes.first).second;
    mp.fatFs = fatFs;

    rc = fatFs->f_mount(&mp.fs, "" /* default disk */, 1 /* immediately */);
    if (FR_OK != rc) {
        ctx->mounts_.erase(emplRes.first);
    }

    return (int)rc;
}

bool
unregister_mount(FatFsVfsCtx* ctx, const char* prefix)
{
    if (nullptr == ctx) {
        ctx = &defaultVfsCtx;
    }
    return ctx->mounts_.erase(prefix);
}

static void
unregister_all_mounts()
{
    for (auto& i : defaultVfsCtx.mounts_) {
        i.second.fatFs->f_unmount("" /* default disk */);
    }
}

static BYTE sqlite3_open_flags_to_fatfs_mode(int flags);
static int fatfs_res_to_sqlite3_res(FRESULT);

////////////////////////////////////////////////////////////////////////////////
int
fsOpen(
  sqlite3_vfs *pVfs,              /* VFS */
  const char *zName,              /* File to open, or 0 for a temp file */
  sqlite3_file *pFile,            /* Pointer to ESP32File struct to populate */
  int flags,                      /* Input SQLITE_OPEN_XXX flags */
  int *pOutFlags                  /* Output SQLITE_OPEN_XXX flags (or NULL) */
)
{
    if (nullptr == zName) {
        return SQLITE_INTERNAL;
    }
    FatFsVfsCtx* ctx = reinterpret_cast<FatFsVfsCtx*>(pVfs->pAppData); 

    // If add an assumption that all mount points is started from '/' and does 
    // not contain another slashes then it'll be possible to simplify search:
    //   1. Find prefix in zName
    //   2. Search it in ctx->mounts_
    // But in common case we have to search all ctx->mounts_ keys in zName one by one.

    auto zLen = strlen(zName);

    fatfs_file_t* f = nullptr;
    const char *filePathOnVolume = nullptr;

    for (auto& i : ctx->mounts_) {
        const char* it = etl::search(zName, zName + zLen, i.first.cbegin(), i.first.cend());
        if (it != zName + zLen) {
            f = reinterpret_cast<fatfs_file_t*>(pFile);
            f->fatFs = i.second.fatFs;
            filePathOnVolume = zName + i.first.size();
            break; 
        }
    }

    if (nullptr == f || nullptr == filePathOnVolume) {
        return SQLITE_INTERNAL;
    }

    BYTE mode = 0;
    FRESULT rc = (f->fatFs)->f_open(&f->fl, filePathOnVolume, sqlite3_open_flags_to_fatfs_mode(flags));
    if (FR_OK != rc) {
        return SQLITE_CANTOPEN;
    }
    if (pOutFlags) {
        *pOutFlags = flags;
    }
    f->base.pMethods = &fs_io_methods;
    return SQLITE_OK;
}

BYTE
sqlite3_open_flags_to_fatfs_mode(int flags)
{
    BYTE mode = 0;

#define CHECK_FLAG(sqlite_flag, fatfs_flag) \
    if (flags & (sqlite_flag)) { \
        mode |= (fatfs_flag); \
    }

    CHECK_FLAG(SQLITE_OPEN_READONLY, FA_OPEN_EXISTING | FA_READ)
    CHECK_FLAG(SQLITE_OPEN_READWRITE, FA_OPEN_EXISTING | FA_READ | FA_WRITE)
    CHECK_FLAG(SQLITE_OPEN_CREATE, FA_CREATE_NEW | FA_CREATE_ALWAYS)

#undef CHECK_FLAG

    return mode;
}

int
fatfs_res_to_sqlite3_res(FRESULT res)
{
    switch (res) {
        case FR_OK: return SQLITE_OK;
        case FR_DISK_ERR: return SQLITE_IOERR;
        case FR_INT_ERR: return SQLITE_INTERNAL;
        case FR_NOT_READY: return SQLITE_IOERR;
        case FR_NO_FILE: return SQLITE_NOTFOUND;
        case FR_NO_PATH: return SQLITE_NOTFOUND;
        case FR_INVALID_NAME: return SQLITE_ERROR;
        case FR_DENIED: return SQLITE_PERM;
        case FR_EXIST: return SQLITE_PERM;
        case FR_INVALID_OBJECT: return SQLITE_INTERNAL;
        case FR_WRITE_PROTECTED: return SQLITE_BUSY;
        case FR_INVALID_DRIVE: return SQLITE_ERROR;
        case FR_NOT_ENABLED: return SQLITE_INTERNAL;
        case FR_NO_FILESYSTEM: return SQLITE_CORRUPT;
        case FR_MKFS_ABORTED: return SQLITE_ABORT;
        case FR_TIMEOUT: return SQLITE_BUSY;
        case FR_LOCKED: return SQLITE_LOCKED;
        case FR_NOT_ENOUGH_CORE: return SQLITE_NOMEM;
        case FR_TOO_MANY_OPEN_FILES: return SQLITE_CANTOPEN;
        case FR_INVALID_PARAMETER: return SQLITE_ERROR;
        default: return SQLITE_INTERNAL;
    }
    return SQLITE_INTERNAL;
}

/// @param[in] zName  File path to delete. Warning: contains prefix meaning one of mounted FatFs
int
fsDelete(sqlite3_vfs* pVfs, const char *zName, int syncDir)
{
    if (nullptr == zName) {
        return SQLITE_INTERNAL;
    }

    FatFsVfsCtx* ctx = reinterpret_cast<FatFsVfsCtx*>(pVfs->pAppData); 
    size_t zLen = strlen(zName);

    FatFs* fatFs = nullptr;
    const char *filePathOnVolume = nullptr;

    for (auto& i : ctx->mounts_) {
        const char* it = etl::search(zName, zName + zLen, i.first.cbegin(), i.first.cend());
        if (it != zName + zLen) {
            filePathOnVolume = zName + i.first.size();
            fatFs = i.second.fatFs;
            break; 
        }
    }

    if (nullptr == fatFs || nullptr == filePathOnVolume) {
        return SQLITE_INTERNAL;
    }

    return fatfs_res_to_sqlite3_res(fatFs->f_unlink(filePathOnVolume));
}

/// Not implemented
int
fsAccess(sqlite3_vfs*, const char *zName, int flags, int *)
{
    return SQLITE_OK;
}

int
fsFullPathname(
  sqlite3_vfs *pVfs,              /* VFS */
  const char *zPath,              /* Input path (possibly a relative path) */
  int nPathOut,                   /* Size of output buffer in bytes */
  char *zPathOut                  /* Pointer to output buffer */
)
{
    if (nullptr == zPathOut
            || nullptr == zPath) {
        return SQLITE_ERROR;
    }
    strncpy(zPathOut, zPath, nPathOut);
    zPathOut[nPathOut-1] = '\0';

    return SQLITE_OK;
}

//
// The following four VFS methods:
//
//   xDlOpen
//   xDlError
//   xDlSym
//   xDlClose
//
// are supposed to implement the functionality needed by SQLite to load
// extensions compiled as shared objects. This simple VFS does not support
// this functionality, so the following functions are no-ops.
//
void
*fsDlOpen(sqlite3_vfs*, const char *zFilename)
{
    return nullptr;
}

void
fsDlError(sqlite3_vfs*, int nByte, char *zErrMsg) 
{
    sqlite3_snprintf(nByte, zErrMsg, "Loadable extensions are not supported");
    zErrMsg[nByte-1] = '\0';
}
void
(*fsDlSym(sqlite3_vfs*,void*, const char *zSymbol))(void)
{
    return nullptr;
}

void
fsDlClose(sqlite3_vfs*, void*)
{
    return;
}

/// TODO: accept special interface pointer which provides such functions and for each
/// platform (AVR, nRF52840, ESP32, etc.) it'll be implemented as derived class

/// Parameter zByte points to a buffer nByte bytes in size. Populate this
/// buffer with pseudo-random data.
/// Not implemented
int
fsRandomness(sqlite3_vfs*, int nByte, char *zOut)
{
    return SQLITE_OK;
}

/// Not implemented
int
fsSleep(sqlite3_vfs*, int microseconds)
{
    return 0;
}

/// Not implemented
int
fsCurrentTime(sqlite3_vfs*, double*)
{
    return SQLITE_OK;
}

int
fsClose(sqlite3_file* pFile)
{
    fatfs_file_t* f = reinterpret_cast<fatfs_file_t*>(pFile);
    return fatfs_res_to_sqlite3_res((f->fatFs)->f_close(&f->fl));
}

int
fsRead(sqlite3_file* pFile,
       void* zBuf,
       int iAmt,
       sqlite3_int64 iOfst)
{
    fatfs_file_t* f = reinterpret_cast<fatfs_file_t*>(pFile);

  /* Flush any data in the write buffer to disk in case this operation
  ** is trying to read data the file-region currently cached in the buffer.
  ** It would be possible to detect this case and possibly save an 
  ** unnecessary write here, but in practice SQLite will rarely read from
  ** a journal file when there is data cached in the write-buffer.
  */
    if (FR_OK != (f->fatFs)->f_sync(&f->fl)) {
        return SQLITE_IOERR_FSYNC;
    }

    if (FR_OK != (f->fatFs)->f_lseek(&f->fl, iOfst)) {
        return SQLITE_IOERR_READ;
    }

    UINT nRead = 0;
    if (FR_OK != (f->fatFs)->f_read(&f->fl, zBuf, iAmt, &nRead)) {
        return SQLITE_IOERR_READ;
    }

    if (nRead == iAmt) {
        return SQLITE_OK;
    } else if (nRead >= 0) {
        if (nRead < iAmt) {
            memset(&((char*)zBuf)[nRead], 0, iAmt - nRead);
        }
        return SQLITE_IOERR_SHORT_READ;
    }

  return SQLITE_IOERR_READ;
}

int
fsWrite(
  sqlite3_file *pFile, /* File handle */
  const void *zBuf,    /* Buffer containing data to write */
  int iAmt,            /* Size of data to write in bytes */
  sqlite_int64 iOfst)  /* File offset to write to */
{
    fatfs_file_t* f = reinterpret_cast<fatfs_file_t*>(pFile);

    if (FR_OK != (f->fatFs)->f_lseek(&f->fl, iOfst)) {
        return SQLITE_IOERR_WRITE;
    }

    UINT bw = 0;
    if (FR_OK != (f->fatFs)->f_write(&f->fl, zBuf, iAmt, &bw)
            || bw != iAmt) {
        return SQLITE_IOERR_WRITE;
    }

    return SQLITE_OK;
}

int
fsTruncate(sqlite3_file* pFile, sqlite3_int64 size)
{
    fatfs_file_t* f = reinterpret_cast<fatfs_file_t*>(pFile);

    if (FR_OK != (f->fatFs)->f_lseek(&f->fl, size)) {
        return SQLITE_IOERR_TRUNCATE;
    }

    if (FR_OK != (f->fatFs)->f_truncate(&f->fl)) {
        return SQLITE_IOERR_TRUNCATE;
    }
    return SQLITE_OK;
}

int
fsSync(sqlite3_file* pFile, [[maybe_unused]] int flags)
{
    fatfs_file_t* f = reinterpret_cast<fatfs_file_t*>(pFile);
    return fatfs_res_to_sqlite3_res((f->fatFs)->f_sync(&f->fl));
}

int
fsFileSize(sqlite3_file* pFile, sqlite3_int64 *pSize)
{
    fatfs_file_t* f = reinterpret_cast<fatfs_file_t*>(pFile);
    *pSize = (f->fatFs)->f_size(&f->fl);
    return SQLITE_OK;
}

/// Not implemented
int
fsLock(sqlite3_file*, int)
{
    return SQLITE_OK;
}

/// Not implemented
int
fsUnlock(sqlite3_file*, int)
{
    return SQLITE_OK;
}

/// Not implemented
int
fsCheckReservedLock(sqlite3_file*, int *pResOut)
{
    return SQLITE_OK;
}

/// Not implemented
int
fsFileControl(sqlite3_file*, int op, void *pArg)
{
    return SQLITE_OK;
}

int
fsSectorSize(sqlite3_file* pFile)
{
    fatfs_file_t* f = reinterpret_cast<fatfs_file_t*>(pFile);
    IO* drv = f->fatFs->getDriver();
    if (nullptr == drv) {
        return 0;
    }
    // TODO: find a way to get FATFS::pdrv for partition which file is located on
    //
    LBA_t sz = 0;
    if (RES_OK != drv->disk_ioctl(0, GET_SECTOR_SIZE, &sz)) {
        return 0;
    }

    return sz;
}

int
fsDeviceCharacteristics(sqlite3_file*)
{
    // The xDeviceCharacteristics()
    // method returns a bit vector describing behaviors of the
    // underlying device:
    // 
    // <ul>
    // <li> [SQLITE_IOCAP_ATOMIC]
    // <li> [SQLITE_IOCAP_ATOMIC512]
    // <li> [SQLITE_IOCAP_ATOMIC1K]
    // <li> [SQLITE_IOCAP_ATOMIC2K]
    // <li> [SQLITE_IOCAP_ATOMIC4K]
    // <li> [SQLITE_IOCAP_ATOMIC8K]
    // <li> [SQLITE_IOCAP_ATOMIC16K]
    // <li> [SQLITE_IOCAP_ATOMIC32K]
    // <li> [SQLITE_IOCAP_ATOMIC64K]
    // <li> [SQLITE_IOCAP_SAFE_APPEND]
    // <li> [SQLITE_IOCAP_SEQUENTIAL]
    // <li> [SQLITE_IOCAP_UNDELETABLE_WHEN_OPEN]
    // <li> [SQLITE_IOCAP_POWERSAFE_OVERWRITE]
    // <li> [SQLITE_IOCAP_IMMUTABLE]
    // <li> [SQLITE_IOCAP_BATCH_ATOMIC]
    // </ul>
    // 
    // The SQLITE_IOCAP_ATOMIC property means that all writes of
    // any size are atomic.  The SQLITE_IOCAP_ATOMICnnn values
    // mean that writes of blocks that are nnn bytes in size and
    // are aligned to an address which is an integer multiple of
    // nnn are atomic.  The SQLITE_IOCAP_SAFE_APPEND value means
    // that when data is appended to a file, the data is appended
    // first then the size of the file is extended, never the other
    // way around.  The SQLITE_IOCAP_SEQUENTIAL property means that
    // information is written to disk in the same order as calls
    // to xWrite().

    // But FatFS does not provide such information and we return zero.
    return 0;
}

} // namespace vfs
} // namespace fatfs

// #if defined(SQLITE_OS_OTHER)

#ifdef __cplusplus
extern "C" {
#endif

SQLITE_API
int
sqlite3_os_init(void)
{
    sqlite3_vfs_register(&fatfs::vfs::default_fatfs_vfs, 1);
    return SQLITE_OK;
}

SQLITE_API
int
sqlite3_os_end(void)
{
    fatfs::vfs::unregister_all_mounts();
    return SQLITE_OK;
}

#ifdef __cplusplus
}  /* End of the 'extern "C"' block */
#endif

// #endif // defined(SQLITE_OS_OTHER)

#pragma GCC diagnostic pop
