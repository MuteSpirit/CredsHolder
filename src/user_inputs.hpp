#pragma once
#ifndef __USER_INPUTS_HPP__
#define __USER_INPUTS_HPP__

#include <inttypes.h>
#include "blind_call.hpp"

// Interface to set callbacks for device inputs (buttons, rotate encoder)
// Should be used to write unit test for UI menu/forms
class UserInputs
{
public:
    /// There are 4 buttons on the device with geometric figures on them:
    // TODO: support button in rotary encoder too
    enum class Button : uint8_t
    {
       square = 0,
       triangle,
       circle,
       cross,
       rotary, // button of rotary encoder
       num_of_buttons
    };

    enum class Encoder : uint8_t
    {
        rotary = 0,
        num_of_encoders
    };

    virtual void set(Button btn, BlindCall cb) = 0; /// on button click (== push+release)
    virtual void unset(Button btn) = 0; /// delete callback previously set on button

    virtual void set(Encoder, BlindCall cb) = 0; /// expected call is cb(int direction)
    virtual void unset(Encoder) = 0; /// delete callback previously set on encoder

    virtual ~UserInputs() = default;
    
protected:
    UserInputs() = default;
};

#endif // !__USER_INPUTS_HPP__
