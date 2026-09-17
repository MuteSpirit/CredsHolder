#pragma once

#ifndef __MENU_HPP__
#define __MENU_HPP__

#include "blind_call.hpp"

class Menu
{
public:
    virtual ~Menu() {};

    virtual void init(BlindCall switchMenuCb) = 0;

    virtual void activate() = 0;
    virtual void deactivate() = 0;

protected:
    Menu() = default;
};

#endif // !__MENU_HPP__
