#if defined(EPOXY_DUINO)

#include <AUnitVerbose.h>
#include "auth_form.cpp"
#include "in_memory_oled.hpp"
#include "imitated_user_unputs.cpp"
#include "auth.hpp"
#include <SHA256.h>

using namespace aunit;

////////////////////////////////////////////////////////////////////////////////
class AuthFormTestHelper
{
public:
    AuthFormTestHelper(AuthForm &form) : form_(form) {}

    void chooseNextSymbolGroup() {form_.chooseNextSymbolGroup();}
    void chooseNextSymbol()      {form_.chooseNextSymbol();}
    void choosePrevSymbol()      {form_.choosePrevSymbol();}
    void eraseLastSymbol()       {form_.eraseLastSymbol();}
    void commitSymbol()          {form_.commitSymbol();}

    void setPassword(const char* pswd) {
        strcpy(form_.password_, pswd);
        form_.allowTryAuth_ = true;
    }
    void commitPassword()        {form_.commitPassword();}

    AuthForm::SymbolGroup typingSymbolGroup() {return form_.typingSymbolGroup_;}
    char typingSymbol() const    {return form_.typingSymbol();}
    const char *password() const {return form_.password_;}
    uint8_t typingGroupLen()     {return form_.typingGroupLen();}

protected:
    AuthForm &form_;
};


////////////////////////////////////////////////////////////////////////////////
class AuthFormTester : public TestOnce
{
protected:
    void setup() override
    {
        TestOnce::setup();

        strcpy(salt, "WithSalt");
        strcpy(saltedPassword, "PasswordWithSalt");
        strcpy(password, "Password");
        strcpy(wrongPassword, "WrongPassword");

        memset(hash, 0, PasswordWandAuth::hashSize);

        SHA256 hasher;
        hasher.reset();
        hasher.update(saltedPassword, strlen(saltedPassword));
        hasher.finalize(hash, PasswordWandAuth::hashSize);

        auth.init(salt, strlen(salt), hash, PasswordWandAuth::hashSize);
    }

protected:
    InMemoryPrint<4, 20> oled;
    ImitatedUserInputs userInputs;
    PasswordWandAuth auth;

    char salt[PasswordWandAuth::hashSize];
    char password[PasswordWandAuth::hashSize];
    char wrongPassword[PasswordWandAuth::hashSize];
    char saltedPassword[PasswordWandAuth::hashSize];
    uint8_t hash[PasswordWandAuth::hashSize];

    AuthForm authForm {oled, userInputs, auth};
    AuthFormTestHelper h {authForm};
};

testF(AuthFormTester, auth_form_ctor)
{
};

testF(AuthFormTester, auth_form_stub_init)
{
    authForm.init(BlindCall::stub(), BlindCall::stub());
};

testF(AuthFormTester, auth_form_deactivate)
{
    struct FakeSwitchFormCb
    {
        uint8_t c {0};
        void call() { ++c; }
    } cb;

    authForm.init(BlindCall::make(&cb, &FakeSwitchFormCb::call), BlindCall::stub());

    assertEqual(0, cb.c);

    authForm.deactivate();
    // click on button which should switch forms between each other
    userInputs.click(UserInputs::Button::triangle);

    assertEqual(1, cb.c);
};

testF(AuthFormTester, auth_form_switch_typing_group)
{
    authForm.init(BlindCall::stub(), BlindCall::stub());
    //
    // Check initial defaults
    assertTrue(AuthForm::SymbolGroup::alphaLowerCase == h.typingSymbolGroup());
    assertEqual('a', h.typingSymbol());
    assertEqual((char)0, h.password()[0]);

    h.chooseNextSymbolGroup();
    assertTrue(AuthForm::SymbolGroup::alphaUpperCase == h.typingSymbolGroup());
    assertEqual('A', h.typingSymbol());
    assertEqual((char)0, h.password()[0]);

    h.chooseNextSymbolGroup();
    assertTrue(AuthForm::SymbolGroup::digit == h.typingSymbolGroup());
    assertEqual('0', h.typingSymbol());
    assertEqual((char)0, h.password()[0]);

    h.chooseNextSymbolGroup();
    assertTrue(AuthForm::SymbolGroup::specials == h.typingSymbolGroup());
    assertEqual(' ', h.typingSymbol());
    assertEqual((char)0, h.password()[0]);
    //
    // cyclic carousel
    h.chooseNextSymbolGroup();
    assertTrue(AuthForm::SymbolGroup::alphaLowerCase == h.typingSymbolGroup());
    assertEqual('a', h.typingSymbol());
    assertEqual((char)0, h.password()[0]);
};

testF(AuthFormTester, auth_form_commit_and_erase_symbol)
{
    authForm.init(BlindCall::stub(), BlindCall::stub());

    assertEqual('a', h.typingSymbol());
    assertEqual((char)0, h.password()[0]);
    //
    // try erase absent symbol
    h.eraseLastSymbol();
    assertEqual('a', h.typingSymbol());
    assertStringCaseEqual("", h.password());
    //
    // Several steps for commit and erase symbol in password
    h.commitSymbol();
    assertEqual('a', h.typingSymbol());
    assertStringCaseEqual("a", h.password());

    h.commitSymbol();
    assertEqual('a', h.typingSymbol());
    assertStringCaseEqual("aa", h.password());

    h.eraseLastSymbol();
    assertEqual('a', h.typingSymbol());
    assertStringCaseEqual("a", h.password());

    h.eraseLastSymbol();
    assertEqual('a', h.typingSymbol());
    assertStringCaseEqual("", h.password());
}

testF(AuthFormTester, auth_form_change_typing_symbol_forward_and_back_one_round)
{
    authForm.init(BlindCall::stub(), BlindCall::stub());

    assertEqual('a', h.typingSymbol());

    h.chooseNextSymbol();
    assertEqual('b', h.typingSymbol());

    h.choosePrevSymbol();
    assertEqual('a', h.typingSymbol());
}

testF(AuthFormTester, auth_form_not_only_a_typing_symbol_become_upper_on_switch_group)
{
    authForm.init(BlindCall::stub(), BlindCall::stub());

    assertTrue(AuthForm::SymbolGroup::alphaLowerCase == h.typingSymbolGroup());
    assertEqual('a', h.typingSymbol());

    h.chooseNextSymbol();
    assertEqual('b', h.typingSymbol());

    h.chooseNextSymbolGroup();
    assertTrue(AuthForm::SymbolGroup::alphaUpperCase == h.typingSymbolGroup());
    assertEqual('B', h.typingSymbol());
}

testF(AuthFormTester, auth_form_change_typing_symbol_forward_and_back_on_different_groups)
{
    authForm.init(BlindCall::stub(), BlindCall::stub());

    h.chooseNextSymbolGroup();

    assertEqual('A', h.typingSymbol());
    assertTrue(AuthForm::SymbolGroup::alphaUpperCase == h.typingSymbolGroup());

    h.chooseNextSymbol();
    assertEqual('B', h.typingSymbol());

    h.choosePrevSymbol();
    assertEqual('A', h.typingSymbol());

    h.chooseNextSymbol();
    assertEqual('B', h.typingSymbol());

    h.chooseNextSymbolGroup();
    assertTrue(AuthForm::SymbolGroup::digit == h.typingSymbolGroup());
    assertEqual('0', h.typingSymbol());

    h.chooseNextSymbol();
    assertEqual('1', h.typingSymbol());

    h.choosePrevSymbol();
    assertEqual('0', h.typingSymbol());

    h.chooseNextSymbolGroup();
    assertTrue(AuthForm::SymbolGroup::specials == h.typingSymbolGroup());
    assertEqual(' ', h.typingSymbol());

    h.chooseNextSymbol();
    assertEqual('/', h.typingSymbol());

    h.choosePrevSymbol();
    assertEqual(' ', h.typingSymbol());
}

testF(AuthFormTester, auth_form_typing_group_len)
{
    authForm.init(BlindCall::stub(), BlindCall::stub());

    h.chooseNextSymbolGroup();
    h.chooseNextSymbolGroup();
    assertTrue(AuthForm::SymbolGroup::digit == h.typingSymbolGroup());
    assertEqual(10, h.typingGroupLen());
}

testF(AuthFormTester, auth_form_choose_next_symbol_is_cyclic_in_forward_order)
{
    authForm.init(BlindCall::stub(), BlindCall::stub());

    h.chooseNextSymbolGroup();
    h.chooseNextSymbolGroup();
    assertEqual('0', h.typingSymbol());

    for (uint8_t i = 0, len = strlen("0123456789"); i < len - 1; ++i) {
        h.chooseNextSymbol();
    }

    assertEqual('9', h.typingSymbol());

    h.chooseNextSymbol();
    assertEqual('0', h.typingSymbol());
};

testF(AuthFormTester, auth_form_choose_next_symbol_is_cyclic_in_reverse_order)
{
    authForm.init(BlindCall::stub(), BlindCall::stub());

    assertEqual('a', h.typingSymbol());

    h.choosePrevSymbol();
    assertEqual('z', h.typingSymbol());

    h.chooseNextSymbolGroup();
    assertEqual('Z', h.typingSymbol());

    h.chooseNextSymbolGroup();
    assertEqual('0', h.typingSymbol());

    h.choosePrevSymbol();
    assertEqual('9', h.typingSymbol());
};

testF(AuthFormTester, auth_form_success_auth)
{
    struct MenuCbAcceptor
    {
        bool accepted_ {false};
        void accept() { accepted_ = true; }
    } cb;
    authForm.init(BlindCall::make(&cb, &MenuCbAcceptor::accept),
                  BlindCall::stub());

    h.setPassword(password);
    h.commitPassword();
    assertTrue(cb.accepted_);
}

testF(AuthFormTester, auth_form_fail_auth)
{
    struct MenuCbAcceptor
    {
        bool accepted_ {false};
        void accept() { accepted_ = true; }
    } cb;
    authForm.init(BlindCall::stub(),
                  BlindCall::make(&cb, &MenuCbAcceptor::accept));

    h.setPassword(wrongPassword);
    h.commitPassword();
    assertTrue(cb.accepted_);
}

testF(AuthFormTester, auth_form_avoid_auth_the_same_correct_password_again)
{
    struct MenuCbAcceptor
    {
        bool accepted_ {false};
        void accept() { accepted_ = true; }
    } cb;
    authForm.init(BlindCall::make(&cb, &MenuCbAcceptor::accept), // success auth cb
                  BlindCall::stub()); // failed auth cb

    h.setPassword(password);

    h.commitPassword();
    assertTrue(cb.accepted_);
    //
    // Try auth with the same password imitating stuck button or accidentiall user button click
    cb.accepted_ = false;
    h.commitPassword();
    assertFalse(cb.accepted_);
}

testF(AuthFormTester, auth_form_avoid_auth_the_same_wrong_password_again)
{
    struct MenuCbAcceptor
    {
        bool accepted_ {false};
        void accept() { accepted_ = true; }
    } cb;
    authForm.init(BlindCall::stub(), // success auth cb
                  BlindCall::make(&cb, &MenuCbAcceptor::accept)); // failed auth cb

    h.setPassword(wrongPassword);

    h.commitPassword();
    assertTrue(cb.accepted_);
    //
    // Try auth with the same wrong password imitating stuck button or accidentiall user button click
    cb.accepted_ = false;
    h.commitPassword();
    assertFalse(cb.accepted_);
}

#endif // EPOXY_DUINO
