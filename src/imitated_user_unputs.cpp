#include "imitated_user_unputs.hpp"


void
ImitatedUserInputs::set(UserAction act, BlindCall cb)
{
    hooks_[static_cast<uint8_t>(act)] = cb;
}

void
ImitatedUserInputs::unset(UserAction act)
{
    hooks_[static_cast<uint8_t>(act)] = BlindCall::stub();
}

void
ImitatedUserInputs::click(UserAction act)
{
    hooks_[static_cast<uint8_t>(act)]();
}
