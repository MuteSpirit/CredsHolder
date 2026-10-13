#pragma once
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
namespace fatfs {

class FatFs;

namespace vfs {

class FatFsVfsCtx;

/// VFS name
const char *name();

/// @return Default FatFsVfsCtx instance
FatFsVfsCtx* vfsCtx();

// TODO: ?maybe register driver instead of FatFs? and be FatFs object owner?
/// Register virtual mount point. 
/// Similar to FatFs string volume ID but with dynamic registration.
/// 
/// @details Should not conflict with FatFs FF_STR_VOLUME_ID option.
/// You MUST disable FF_STR_VOLUME_ID.
/// There is FatFsVfsCtx class with mount point list. One static instance is defined by default (@see vfsCtx).
///
/// @param[in] ctx VFS context pointer. If equal NULL then default context will be used.
/// @param[in] fatfs Pointer to FatFs class with set driver. Partitions must have FS before this call. Run f_mkfs preliminary.
/// @param[in] prefix String with absolute path suffix which will play role of mount point. 
/// You should use that `prefix` in sqlite3 db file path to choose needed FatFs.
///
/// @return true if success and false otherwise
// TODO: add IN param for FRESULT
int register_mount(FatFsVfsCtx* ctx, FatFs *fatfs, const char* prefix);


/// Unregister virtual mount point. 
/// @param[in] ctx VFS context pointer. If equal NULL then default context will be used.
bool unregister_mount(FatFsVfsCtx* ctx, const char* prefix);

} // namespace vfs
} // namespace fatfs
