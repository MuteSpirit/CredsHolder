#pragma once
#ifndef __IMITATED_USER_INPUTS_HPP__
#define __IMITATED_USER_INPUTS_HPP__

#include "user_inputs.hpp"

class ImitatedUserInputs : public UserInputs
{
public:
    ImitatedUserInputs() = default;
    ~ImitatedUserInputs() = default;
    
    virtual void set(UserInputs::Button btn, BlindCall cb) override;
    virtual void unset(UserInputs::Button btn) override;

    virtual void set(UserInputs::Encoder, BlindCall cb) override;
    virtual void unset(UserInputs::Encoder) override;

    void click(UserInputs::Button btn);
    void rotate(UserInputs::Encoder, int direction);

protected:
    struct ButtonHook
    {
        BlindCall cb_     {BlindCall::stub()};
        bool push_happen_ {false};
    };

    ButtonHook btnHooks_[static_cast<uint8_t>(UserInputs::Button::num_of_buttons)];

    BlindCall encoderHooks_[static_cast<uint8_t>(UserInputs::Encoder::num_of_encoders)] {BlindCall::stub()};
};

#endif // !__IMITATED_USER_INPUTS_HPP__
