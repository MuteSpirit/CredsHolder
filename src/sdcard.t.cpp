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
//
// ================================================================================
// Arduino-FatFs Wrapper License (MIT)
// ================================================================================
//
// MIT License
//
// Copyright (c) 2022 Phil Schatzmann
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#if defined(EPOXY_DUINO)
#include "creds_holder.hpp"

// Avoid double definition:
//   fatfs.h includes ff.h which includes ff-inc.h which again include ff.h
//   FATFS_HEADER_ONLY_IMPL definition allows avoid include ff-inc.h by ff.h
#define FATFS_HEADER_ONLY_IMPL
#include <fatfs.h>

#undef ARDUINO
#include <driver/FileIO.h>
#define ARDUINO

#include "sqlite.hpp"

#include "sqlite3_vfs_over_fatfs.hpp"

PARTITION VolToPart[] {
    {0, 0}, // 0: -> 1st physical disk, 1st FAT partition
    {0, 1}  // 1: -> 1st physical disk, 2nd FAT partition
}; /* Volume - Partition mapping table */

// Must be included as the last one to avoid troubles with macro "test"
// when such word is used in headers above
#include <AUnitVerbose.h>

test(fatfs_fat_partition)
{
    fatfs::RamIO drv{1024 /*sectors*/, 512 /*bytes per sector*/};

    fatfs::FatFs fs(drv);

    struct fatfs::MKFS_PARM fsParm {
        .fmt = FM_FAT | FM_FAT32,
        .n_fat = 0, // The default value (0) and any invalid value gives 1.
        .align = 0, // If a zero (the default value) or an invalid value is given, the function obtains the block size from lower layer with disk_ioctl function
        .n_root = 0, // The default value (0) and any invalid value gives 512
        .au_size = 512
    };

    uint8_t work[FF_MAX_SS] = {0};

    assertTrue(fatfs::FR_OK == fs.f_mkfs("0:", &fsParm, work, sizeof(work)));
}

test(fatfs_mbr_and_two_fat_partitions)
{
    fatfs::RamIO drv{1024 /*sectors*/, 512 /*bytes per sector*/};
    fatfs::FatFs fs(drv);
    uint8_t work[FF_MAX_SS] = {0};
    //
    // Create MBR
    LBA_t plist[] = {50, 50, 0}; // 50% - for 1st partition and 50% - for 2nd one
    assertTrue(fatfs::FR_OK == fs.f_fdisk(0, plist, work));
    //
    // Format partitions
    struct fatfs::MKFS_PARM fsParm {
        .fmt = FM_FAT | FM_FAT32,
        .n_fat = 0, // The default value (0) and any invalid value gives 1.
        .align = 0, // If a zero (the default value) or an invalid value is given, the function obtains the block size from lower layer with disk_ioctl function
        .n_root = 0, // The default value (0) and any invalid value gives 512
        .au_size = 512
    };

    assertTrue(fatfs::FR_OK == fs.f_mkfs("0:", &fsParm, work, sizeof(work)));
    assertTrue(fatfs::FR_OK == fs.f_mkfs("1:", &fsParm, work, sizeof(work)));
    //
    // Mount partitions
    // This is required preliminary step in TinyFATFS to be able set partition label
    fatfs::FATFS fs1;
    assertTrue(fatfs::FR_OK == fs.f_mount(&fs1, "0:", 1 /*immediately*/));

    fatfs::FATFS fs2;
    assertTrue(fatfs::FR_OK == fs.f_mount(&fs2, "1:", 1 /*immediately*/));
    //
    // Set partitions labels
    assertTrue(fatfs::FR_OK == fs.f_setlabel("0:apps"));
    assertTrue(fatfs::FR_OK == fs.f_setlabel("1:storage"));
    //
    // Create files on each
    fatfs::FIL f;
    assertTrue(fatfs::FR_OK == fs.f_open(&f, "0:f1.txt", FA_OPEN_EXISTING | FA_CREATE_NEW | FA_WRITE));

    UINT result = 0;
    const char *buf = "test";
    assertTrue(fatfs::FR_OK == fs.f_write(&f, buf, strlen(buf)+1, &result));
    assertEqual(strlen(buf)+1, (size_t)result);
    
    assertTrue(fatfs::FR_OK == fs.f_close(&f));
    //
    // unmount
    // FIXME: move to teardown
    assertTrue(fatfs::FR_OK == fs.f_unmount("0:"));
    assertTrue(fatfs::FR_OK == fs.f_unmount("1:"));
}

test(sqlite3_with_fatfs_vfs)
{
    fatfs::RamIO drv{1024 /*sectors*/, 512 /*bytes per sector*/};
    // Replace with FileIO to touch that file yourself, mount and check file
    // fatfs::FileIO drv {"/tmp/test.fat.img", 20480, 512};
    uint8_t work[FF_MAX_SS] = {0};

    fatfs::FatFs fs(drv);
    //
    // Create MBR
    LBA_t plist[] = {50, 50, 0};
    assertTrue(fatfs::FR_OK == fs.f_fdisk(0, plist, work));
    // Create FS
    assertTrue(fatfs::FR_OK == fs.f_mkfs("0:", nullptr, work, sizeof(work)));
    assertTrue(fatfs::FR_OK == fs.f_mkfs("1:", nullptr, work, sizeof(work)));

    assertTrue(fatfs::FR_OK == fatfs::vfs::register_mount(nullptr, &fs, "/rom0"));

    assertTrue(SQLITE_OK == sqlite3_initialize());

    sqlite3 *db = nullptr;
    assertTrue(SQLITE_OK == sqlite3_open_v2("/rom0/test.db", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, fatfs::vfs::name()));
    assertNotEqual(nullptr, db);

    {
        const char *sql =
            "CREATE TABLE IF NOT EXISTS accounts ("
            "  id       INTEGER PRIMARY KEY AUTOINCREMENT,"
            "  name     TEXT    NOT NULL,"
            "  url      TEXT    NOT NULL,"
            "  username TEXT    NOT NULL,"
            "  password TEXT    NOT NULL"
            ");";

        assertEqual(SQLITE_OK, sqlite3_exec(db, sql, nullptr, nullptr, nullptr));
    }
    { // Insert test records using prepared statements 
        const char *insert_sql =
            "INSERT INTO accounts (name, url, username, password) VALUES (?, ?, ?, ?);";

        /* Test data: name, url, username, password */
        const char *test_data[][4] = {
            { "GitHub",       "https://github.com",       "alice_dev",    "s3cr3tP@ss" },
            { "Gmail",        "https://mail.google.com",  "alice@gmail.com", "hunter2!" },
            { "AWS Console",  "https://aws.amazon.com",   "alice-admin",  "Cl0ud$afe2024" },
            { "StackOverflow","https://stackoverflow.com","alice_coder",  "Qwerty!2345" },
            { "Bank Portal",  "https://mybank.example",   "alice.bank",   "Mon3y$Tree!" }
        };
        const int num_records = sizeof(test_data) / sizeof(test_data[0]);

        sqlite3_stmt *stmt = NULL;
        assertEqual(SQLITE_OK, sqlite3_prepare_v2(db, insert_sql, -1, &stmt, NULL));

        for (int i = 0; i < num_records; i++) {
            sqlite3_bind_text(stmt, 1, test_data[i][0], -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, test_data[i][1], -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, test_data[i][2], -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 4, test_data[i][3], -1, SQLITE_TRANSIENT);

            assertEqual(SQLITE_DONE, sqlite3_step(stmt));

            sqlite3_reset(stmt);
            sqlite3_clear_bindings(stmt);
        }
        sqlite3_finalize(stmt);
    }
    { // Query: list all records
        const char* sql = "SELECT id, name, url, username, password FROM accounts ORDER BY id;";
        assertEqual(SQLITE_OK, sqlite3_exec(db, sql, nullptr, nullptr, nullptr));
    }

    sqlite3_close_v2(db);

    assertTrue(SQLITE_OK == sqlite3_shutdown());

    assertTrue(fatfs::vfs::unregister_mount(nullptr, "/rom0"));

    assertTrue(fatfs::FR_OK == fs.f_unmount(""));
}

#endif // defined(EPOXY_DUINO)
