#include "settings_menu.hpp"

// GyverOLEDMenu library draw menu item ugly and was replaced
// #define MENU_SELECTED_H  (-5)
// #define MENU_ITEM_PADDING_TOP 10
// #include "GyverOLEDMenu.h"

// #include "SSD1306Ascii.h" // OLED fonts

#include "device_inputs.hpp"
#include "settings.hpp"
#include "cli.hpp"
#include "oled.hpp"


SettingsMenu::SettingsMenu(Oled &oled, DeviceInputs& userInputs, Settings& settings)
    : oled_(oled)
    , userInputs_(userInputs)
    , settings_(settings)
{
    uint8_t i = 0;

    items_[i].title_ = "CLI";
    items_[i].type_ = MenuItem::Type::bool_t;
    items_[i].bValue_ = settings_.cli_turn_on_;
    items_[i].targetValue_ = &settings_.cli_turn_on_;
    ++i;
    items_[i].title_ = "Unhide Pass";
    items_[i].type_ = MenuItem::Type::bool_t;
    items_[i].bValue_ = settings_.unhide_passwords_;
    items_[i].targetValue_ = &settings_.unhide_passwords_;

    // TODO: add "factory reset" and "logout"
}

void
SettingsMenu::init(BlindCall nextMenuCb, BlindCall prevMenuCb)
{
    nextMenuCb_ = nextMenuCb;
    prevMenuCb_ = prevMenuCb;
}

void
SettingsMenu::selectNextItem()
{
    if (activeItemIdx_ < numItems_ - 1) {
        ++activeItemIdx_;
        draw();
    }
}

void
SettingsMenu::selectPrevItem()
{
    if (activeItemIdx_ > 0) {
        --activeItemIdx_;
        draw();
    }
}

void
SettingsMenu::navigateItemCb(int direction)
{
    if (0 == direction) {
        return;
    }
    if (direction > 0) {
        selectNextItem();
    } else {
        selectPrevItem();
    }
}

void
SettingsMenu::selectValue(int direction)
{
    if (0 == direction) {
        return;
    }
    if (direction > 0) {
        selectNextValue();
    } else {
        selectPrevValue();
    }
}

void
SettingsMenu::toggleChangeItem()
{
    if (editMode_) {
        leaveEditMode();
    } else {
        enterEditMode();
    }
}

void
SettingsMenu::enterEditMode()
{
    editMode_ = true;

    // oled_.setFont(cp437font8x8);

    userInputs_.set(DeviceInputs::UserAction::left, BlindCall::make(this, &SettingsMenu::selectPrevValue));
    userInputs_.set(DeviceInputs::UserAction::down, BlindCall::make(this, &SettingsMenu::selectNextValue));

    userInputs_.set(DeviceInputs::UserAction::right, BlindCall::make(this, &SettingsMenu::commitChange));
    userInputs_.set(DeviceInputs::UserAction::up, BlindCall::make(this, &SettingsMenu::cancelChange));

    userInputs_.set(DeviceInputs::UserAction::enter, BlindCall::make(this, &SettingsMenu::selectValue));

    draw();
}

void
SettingsMenu::leaveEditMode()
{
    editMode_ = false;

    // oled_.setFont(System5x7);

    userInputs_.set(DeviceInputs::UserAction::up, BlindCall::make(this, &SettingsMenu::selectPrevItem));
    userInputs_.set(DeviceInputs::UserAction::down, BlindCall::make(this, &SettingsMenu::selectNextItem));

    userInputs_.set(DeviceInputs::UserAction::right, BlindCall::make(this, &SettingsMenu::toggleChangeItem));
    userInputs_.set(DeviceInputs::UserAction::left, prevMenuCb_);

    // userInputs_.set(DeviceInputs::UserAction::rotary, BlindCall::make(this, &SettingsMenu::navigateItemCb));

    draw();
}

void
SettingsMenu::selectPrevValue()
{
    items_[activeItemIdx_].bValue_ = !items_[activeItemIdx_].bValue_;
    draw();
}

void
SettingsMenu::selectNextValue()
{
    items_[activeItemIdx_].bValue_ = !items_[activeItemIdx_].bValue_;
    draw();
}

void
SettingsMenu::commitChange()
{
    if (*items_[activeItemIdx_].targetValue_ != items_[activeItemIdx_].bValue_) {
        if (0 == activeItemIdx_) { // TODO: make action per menu item
            if (items_[activeItemIdx_].bValue_) {
                cli_on();
            } else {
                cli_off();
            }
        }
    }

    *items_[activeItemIdx_].targetValue_ = items_[activeItemIdx_].bValue_;
    leaveEditMode();
}

void
SettingsMenu::cancelChange()
{
    items_[activeItemIdx_].bValue_ = *items_[activeItemIdx_].targetValue_;
    leaveEditMode();
}

void
SettingsMenu::draw()
{
    oled_.clear();
    oled_.home();

    if (editMode_) {
        oled_.println(F("<edit mode>"));
    }
    oled_.println(items_[activeItemIdx_].title_);
    oled_.println(F(""));
    // TODO: handle value type
    oled_.println(items_[activeItemIdx_].bValue_ ? "On" : "Off");
}

void
SettingsMenu::activate()
{
    cancelChange();
}

void
SettingsMenu::deactivate()
{
    userInputs_.unset(DeviceInputs::UserAction::right);
    userInputs_.unset(DeviceInputs::UserAction::up);
    userInputs_.unset(DeviceInputs::UserAction::left);
    userInputs_.unset(DeviceInputs::UserAction::down);

    // userInputs_.unset(DeviceInputs::UserAction::rotary);

    oled_.clear();

    editMode_ = false;
    // oled_.setFont(System5x7);
}
