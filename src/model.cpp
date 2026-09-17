#include "model.hpp"

template<>
char *
get_key_ptr(Account &o)
{
    return o.name;
}

template<>
uint8_t
get_key_size<Account>()
{
    return sizeof(Account::name);
}

template<>
ptrdiff_t
get_key_offset<Account>()
{
    return offsetof(Account, name);
}
