#include "accounts_menu.hpp"

#include "blind_call.hpp"
#include "device.hpp"
#include "keyboard.hpp"
#include "model_storage.hpp"
#include "settings.hpp"
#include "oled.hpp"


AccountsMenu::AccountsMenu(Keyboard& keyboard,
                           Oled& oled,
                           DeviceInputs& userInputs,
                           const Settings& settings,
                           ModelStorage<Account>& modelStore)
    : keyboard_(keyboard)
    , oled_(oled)
    , userInputs_(userInputs)
    , settings_(settings)
    , modelStore_(modelStore)
{
    memset(&acc_, 0, sizeof(acc_));
}

void
AccountsMenu::init(BlindCall switchMenuCb)
{
    // we don't touch switch_menu callback on "triangle" button so far, so don't remember that
    (void)(switchMenuCb);

    acc_idx_ = 0;
    memset(&acc_, 0, sizeof(Account));
}

void
AccountsMenu::activate()
{
    userInputs_.set(DeviceInputs::UserAction::left, BlindCall::make(this, &AccountsMenu::sendUsername));
    userInputs_.set(DeviceInputs::UserAction::down, BlindCall::make(this, &AccountsMenu::sendPassword));
    userInputs_.set(DeviceInputs::UserAction::right, BlindCall::make(this, &AccountsMenu::sendTab));
    userInputs_.set(DeviceInputs::UserAction::up, BlindCall::make(this,&AccountsMenu::navigateAccounts));

    oled_.home();

    acc_idx_ = 0;
    memset(&acc_, 0, sizeof(Account));

    if (modelStore_.get(acc_idx_, acc_) || 
        modelStore_.getNext(acc_idx_, acc_, acc_idx_)) {
        draw();
    } else {
        oled_.println(F("No creds accounts"));
    }
}

void
AccountsMenu::deactivate()
{
    userInputs_.unset(DeviceInputs::UserAction::left);
    userInputs_.unset(DeviceInputs::UserAction::down);
    userInputs_.unset(DeviceInputs::UserAction::right);
    userInputs_.unset(DeviceInputs::UserAction::enter);

    acc_idx_ = 0;
    memset(&acc_, 0, sizeof(Account));

    oled_.clear();
}


void
AccountsMenu::sendTab()
{
    keyboard_.push_tab();
}

void
AccountsMenu::sendUsername()
{
    keyboard_.print(acc_.username);
}

void
AccountsMenu::sendPassword()
{
    keyboard_.print(acc_.password);
}

/// @param[in] direction - 0 = No rotation, 1 = Clockwise, -1 = Counter Clockwise
void
AccountsMenu::navigateAccounts(int direction)
{
    if (0 == direction) {
        return;
    }

    ModelStorage<Account>::ObjIndex idx = acc_idx_;

    do {
        if (direction > 0) {
            if (modelStore_.getNext(acc_idx_, acc_, idx)) {
                break;
            }
            if (modelStore_.get(0, acc_)) {
                idx = 0;
                break;
            }
            if (modelStore_.getNext(0, acc_, idx)) {
                break;
            }
        } else {
            if (modelStore_.getPrev(acc_idx_, acc_, idx)) {
                break;
            }
            auto maxIdx = modelStore_.maxIdx();

            if (modelStore_.get(maxIdx, acc_)) {
                idx = maxIdx;
                break;
            }
            if (modelStore_.getPrev(maxIdx, acc_, idx)) {
                break;
            }
        }
    } while (0);

    if (idx != acc_idx_) {
        acc_idx_ = idx;

        oled_.clear();
        oled_.home();

        draw();
    }
}

void
AccountsMenu::draw()
{
    oled_.println("  Credentials:  ");

    oled_.print(F("N: "));
    oled_.println(acc_.name);

    oled_.print(F("U: "));
    oled_.println(acc_.username);

    oled_.print(F("P: "));
    if (settings_.unhide_passwords_) {
        oled_.println(acc_.password);
    } else {
        char hidden_passwd[PASSWORD_SIZE] = {0};
        const uint8_t len = strnlen(acc_.password, PASSWORD_SIZE);
        for (uint8_t i = 0; i < len; ++i) {
            hidden_passwd[i] = '*';
        }
        oled_.println(hidden_passwd);
    }
}
