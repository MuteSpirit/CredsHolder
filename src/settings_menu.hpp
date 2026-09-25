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
#pragma once

#include "menu.hpp"

class Settings;
class DeviceInputs;
class Oled;


class SettingsMenu : public Menu
{
public:
    SettingsMenu(Oled &oled, DeviceInputs& userInputs, Settings& settings);
    ~SettingsMenu() = default;

    virtual void init(BlindCall nextMenuCb, BlindCall prevMenuCb) override;

    virtual void activate() override;
    virtual void deactivate() override;

    virtual void draw() override;
protected:
    struct MenuItem // TODO: move to base class
    {
        const char* title_;

        enum Type
        {
            bool_t,
            action_t
        } type_;

        union
        {
            struct
            {
                bool bValue_;
                volatile bool *targetValue_;
            };
            struct
            {
                void (*cb_)(void *ctx);
                void *ctx_;
            };
        };
    };

protected:
    void selectPrevItem();
    void selectNextItem();
    void navigateItemCb(int direction);

    void toggleChangeItem();

    void selectPrevValue();
    void selectNextValue();
    void selectValue(int direction);

    void enterEditMode();
    void leaveEditMode();

    void commitChange();
    void cancelChange();

protected:
    Oled &oled_;
    DeviceInputs& userInputs_;
    Settings &settings_;

    static const uint8_t numItems_{2};
    MenuItem items_[numItems_];
    uint8_t activeItemIdx_{0};
    bool editMode_{false};

    BlindCall nextMenuCb_ {BlindCall::stub()};
    BlindCall prevMenuCb_ {BlindCall::stub()};
};
