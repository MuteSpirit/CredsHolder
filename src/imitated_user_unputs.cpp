#include "imitated_user_unputs.hpp"

void
ImitatedUserInputs::set(UserInputs::Button btn, BlindCall cb)
{
    btnHooks_[static_cast<uint8_t>(btn)].cb_ = cb;
}

void
ImitatedUserInputs::unset(UserInputs::Button btn)
{
    btnHooks_[static_cast<uint8_t>(btn)].cb_ = BlindCall::stub();
}

void
ImitatedUserInputs::set(UserInputs::Encoder enc, BlindCall cb)
{
    encoderHooks_[static_cast<uint8_t>(enc)] = cb;
}

void
ImitatedUserInputs::unset(UserInputs::Encoder enc)
{
    encoderHooks_[static_cast<uint8_t>(enc)] = BlindCall::stub();
}

void
ImitatedUserInputs::click(UserInputs::Button btn)
{
    btnHooks_[static_cast<uint8_t>(btn)].cb_();
}

void
ImitatedUserInputs::rotate(UserInputs::Encoder encoder, int direction)
{
    encoderHooks_[static_cast<uint8_t>(encoder)](direction);
}
