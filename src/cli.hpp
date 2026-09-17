#pragma once
#ifndef __CLI_HPP__
#define __CLI_HPP__

struct Account;

void cli_init(const bool cli_turn_on);
void cli_loop_step();

void cli_on();
void cli_off();

#endif // __CLI_HPP__
