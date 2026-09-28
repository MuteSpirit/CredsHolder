// CredsHolder (Hardware Credential Manager)
// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
#if defined(EPOXY_DUINO)

#include "blind_call.cpp"

// Must be included as the last header to avoid troubles with macro "test"
// when such word is used in headers above
#include <AUnitVerbose.h>

test(blind_call_default_ctor)
{
    BlindCall cb(BlindCall::stub());
    cb();
}

test(blind_call_stub_or_not)
{
    BlindCall stubCb(BlindCall::stub());
    assertFalse(stubCb);

    struct A {
        void foo() {}
    };
    A a;
    BlindCall nonStubCb(BlindCall::make(&a, &A::foo));
    assertTrue(nonStubCb);
}

test(blind_call_cb_no_args)
{
    class NoArgs
    {
    public:
        void foo() {
            ++c;
        }

        uint8_t c {0};
    };

    NoArgs a;
    assertEqual(0, a.c);
    
    BlindCall bc(BlindCall::make(&a, &NoArgs::foo));
    bc();

    assertEqual(1, a.c);
};

test(blind_call_cb_with_one_arg)
{
    class OneArg
    {
    public:
        void foo(const uint8_t lhs) {
            c += lhs;
        }

        uint8_t c {0};
    };

    OneArg a;
    assertEqual(0, a.c);
    
    BlindCall bc(BlindCall::make(&a, &OneArg::foo));
    bc(2);

    assertEqual(2, a.c);
};

test(blind_call_cb_with_two_args)
{
    class TwoArgs
    {
    public:
        void foo(const uint8_t lhs, const char b) {
            (void)(b);
            c += lhs;
        }

        uint8_t c {0};
    };

    TwoArgs a;
    assertEqual(0, a.c);
    
    BlindCall bc(BlindCall::make(&a, &TwoArgs::foo));
    bc(3, 'a');

    assertEqual(3, a.c);
};

test(blind_call_several_bc_to_the_same_class_and_different_members)
{
    class VariadicArgs
    {
    public:
        void foo() {
            ++c;
        }
        void bar(const uint8_t lhs) {
            c += lhs;
        }
        void nop(const float lhs) {
            c += floor(lhs);
        }

        uint8_t c {0};
    };

    VariadicArgs a;
    assertEqual(0, a.c);
    
    BlindCall bcFoo(BlindCall::make(&a, &VariadicArgs::foo));
    bcFoo();
    assertEqual(1, a.c);

    BlindCall bcBar(BlindCall::make(&a, &VariadicArgs::bar));
    bcBar(2);
    assertEqual(3, a.c);

    BlindCall bcNop(BlindCall::make(&a, &VariadicArgs::nop));
    bcNop((float)3.1);
    assertEqual(6, a.c);
};

#endif // EPOXY_DUINO
