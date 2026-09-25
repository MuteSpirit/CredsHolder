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
// This file is based on idea of selection list at U8g2 library:
/*
  selection list with scroll option
  
  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
*/
// #include "ui_selection_list.hpp"
// #include "creds_holder.hpp"
//
//
// // #include "ssd1306_oled.hpp"
//
// ////////////////////////////////////////////////////////////////////////////////
// template<typename Iterator>
// class UISelectionListImpl
// {
// public:
//     UISelectionListImpl(const char* title,
//                         Oled& oled,
//                         Iterator start,
//                         Iterator end);
//
//     const char* selected();
//     void next();
//     void prev();
//
//     void draw();
//
// public:
//     etl::string<255> title_;
//     Oled& oled_;
//     uint8_t visible_;
//     uint8_t x_{0};
//     uint8_t y_{0};
//
//     size_t total_{0};
//
//     Iterator firstItemIt_;
//     Iterator endItemIt_;
//
//     Iterator firstIt_;
//     Iterator curIt_;
// };
//
// ////////////////////////////////////////////////////////////////////////////////
// // etl::unique_ptr<UISelectionList>
// // new_sl(Oled& oled, 
// //        UISelectionList::Iterator start,
// //        UISelectionList::Iterator end)
// // {
// //     SSD1306I2C* ssd1306 = dynamic_cast<SSD1306I2C*>(&oled);
// //     if (!ssd1306) {
// //         return etl::unique_ptr<UISelectionList>(nullptr);
// //     }
// //
// //     etl::unique_ptr<UISelectionList> ptr(new UISelectionList(ssd1306->u8x8_, start, end));
// //
// //     return ptr;
// // }
//
// UISelectionList::UISelectionList(const char* title, Oled& oled, Iterator start, Iterator end)
// {
//     new(impl_) UISelectionListImpl(title, oled, start, end);
// }
//
// UISelectionListImpl::UISelectionListImpl(const char* title,
//                                          Oled& oled,
//                                          UISelectionList::Iterator start,
//                                          UISelectionList::Iterator end)
//     : title_(title)
//     , oled_(oled)
//     , firstItemIt_(start)
//     , endItemIt_(end)
//     , firstIt_(start)
//     , curIt_(start)
//     , visible_(oled.getRows())
//     , total_(etl::distance_helper(start, end, std::bidirectional_iterator_tag()))
// {
// }
//
// ////////////////////////////////////////////////////////////////////////////////
// const char* 
// UISelectionList::selected()
// {
//     return impl()->selected();
// }
//
// UISelectionListImpl*
// UISelectionList::impl()
// {
//     return reinterpret_cast<UISelectionListImpl*>(impl_);
// }
//
// const char* 
// UISelectionListImpl::selected()
// {
//     return *curIt_;
// }
//
// ////////////////////////////////////////////////////////////////////////////////
// void
// UISelectionList::next()
// {
//     impl()->next();
// }
//
// void
// UISelectionListImpl::next()
// {
//     ++curIt_;
//     if (curIt_ == endItemIt_) {
//         curIt_ = firstItemIt_;
//         firstIt_ = firstItemIt_;
//     } else {
//         // TODO: check me
//         if (etl::distance_helper(firstIt_, curIt_, std::bidirectional_iterator_tag()) > visible_) {
//             ++firstIt_;
//         }
//     }
// }
//
// ////////////////////////////////////////////////////////////////////////////////
// void
// UISelectionList::prev()
// {
//     impl()->prev();
// }
//
// void
// UISelectionListImpl::prev()
// {
//     if (curIt_ == firstItemIt_) {
//         curIt_ = etl::prev(endItemIt_);
//
//         firstIt_ = firstItemIt_;
//         if (total_ > visible_) {
//             firstIt_ = etl::distance_helper(endItemIt_, -visible_, std::bidirectional_iterator_tag())
//         }
//     } else {
//         if (firstIt_ == curIt_) {
//             --firstIt_;
//         }
//         --curIt_;
//     }
// }
//
// void
// UISelectionList::draw()
// {
//     impl()->draw();
// }
//
// void
// UISelectionListImpl::draw()
// {
//     oled_.setInverseFont(0);
//
//     if (!title_.empty()) {
//         oled_.drawUTF8(x_, y_, title_);
//         ++y;
//         --visible_;
//     }
//
//     // if ( u8sl.current_pos >= u8sl.total )
//     //   u8sl.current_pos = u8sl.total-1;
//
//     Iterator it = firstIt_;
//     for(uint8_t i = 0; i < visible_; ++i, ++it) {
//         if (it == curIt_) {
//             oled_.setInverseFont(1);
//         }
//         oled_.drawUTF8(x_, y_ + i, *it);
//         if (it == curIt_) {
//             oled_.setInverseFont(0);
//         }
//     }
// }
