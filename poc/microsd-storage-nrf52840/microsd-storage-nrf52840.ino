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

#include <fatfs.h>

RamIO drv{200 /*sectors*/, 512 /*bytes per sector*/};

void setup() {
  Serial.begin(115200);
  Serial.println("SD begin");

  // start SD
  if (!SD.begin(drv)){
    Serial.println("SD.begin() error");
    while(true);
  }

  Serial.println("Opening file...");
  File file = SD.open("test", FILE_WRITE);
  if (!file){
    Serial.println("Could not create file");
    while(true);
  }

  // printing data
  file.println("test");
  file.flush();

  // reading back the result
  Serial.println("Reading file...");
  file.seek(0);
  uint8_t data[100] = {0};
  int len = file.readBytes(data, 100);
  Serial.print("bytes read: ");
  Serial.println(len);
  Serial.print("data: ");
  Serial.println((char*)data);
  Serial.print("file size: ");
  Serial.println(file.size());

  file.close();


}

void loop() {}

