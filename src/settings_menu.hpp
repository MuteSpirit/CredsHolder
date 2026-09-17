#pragma once
#ifndef __SETTINGS_MENU_HPP__
#define __SETTINGS_MENU_HPP__

#include "menu.hpp"

class Settings;
class DeviceInputs;
class Oled;


class SettingsMenu : public Menu
{
public:
    SettingsMenu(Oled &oled, DeviceInputs& userInputs, Settings& settings);
    ~SettingsMenu() = default;

    virtual void init(BlindCall switchMenuCb) override;

    virtual void activate() override;
    virtual void deactivate() override;

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
    void drawItem();

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

    BlindCall switchMenuCb_;
};

#endif // !__SETTINGS_MENU_HPP__
