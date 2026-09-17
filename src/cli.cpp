#include <SimpleCLI.h>
#include <c/arg.h>

#include "cli.hpp"
#include "model.hpp"
#include "version.hpp"
#include "model_storage.hpp"

/******************************************************************************/
// from SimpleCLI/c/arg.c
const char*
arg_get_value(arg* a)
{
    if (a) {
        if (a->val) return a->val;
        if (a->default_val) return a->default_val;
    }
    return "";
}

/******************************************************************************/
const char * const add_cmd_usage PROGMEM = "Usage: add <username> [password]. Add new account. If \"password\" is absent it'll be generated";
const char * const del_cmd_usage PROGMEM = "Usage: rm <username>. Delete account.";
const char * const ls_cmd_usage  PROGMEM = "Usage: ls. Print exist accounts.";

// Use such long name and not use aliases to avoid mistake during this command usage
const char * const factory_cmd_usage PROGMEM = "Usage: factory_reset. Perform factory reset. !!! All accounts and settings will be lost!!!";

const char * const usage[] = {
    add_cmd_usage,
    del_cmd_usage,
    ls_cmd_usage,
    factory_cmd_usage
};

#define USAGE_SIZE  (sizeof(usage) / sizeof(usage[0]))

// TODO: SimpleCLI(int commandQueueSize = 10, int errorQueueSize = 10); 
SimpleCLI cli;
extern ModelStorage<Account> modelStore;

void print_usage(cmd *pcmd);
void add_account(cmd *pcmd);
void del_account(cmd *pcmd);
void ls_accounts(cmd *pcmd);
void factory_reset(cmd *pcmd);

void error_cb(cmd_error* e);

void
cli_init(const bool cli_turn_on)
{
    cli.addCommand("h/e/l/p", print_usage);

    Command add_cmd = cli.addBoundlessCommand("a/d/d", add_account);
    add_cmd.setDescription(add_cmd_usage);

    add_cmd.addPositionalArgument("username");
    add_cmd.addPositionalArgument("password");

    Command del_cmd = cli.addSingleArgCmd("r/m,d/e/l", del_account);
    del_cmd.setDescription(del_cmd_usage);

    del_cmd.addPositionalArgument("username");

    Command ls_cmd = cli.addCommand("l/s", ls_accounts);
    ls_cmd.setDescription(ls_cmd_usage);

    Command factory_cmd = cli.addCommand("factory_reset", factory_reset);
    factory_cmd.setDescription(factory_cmd_usage);

    cli.setOnError(error_cb);

    if (cli_turn_on) {
        cli_on();
    } else {
        cli_off();
    }
}

void cli_loop_step() {
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');

        if (input.length() > 0) {
            Serial.print("# ");
            Serial.println(input);

            cli.parse(input);
        }
    }
}

/// Print error message to user if he made a mistake
/// If try to get human readable error message via getString() method then sketch size increases too much
void
error_cb(cmd_error* e)
{
    if (!Serial) {
        return;
    }
    CommandError ce(e);
    if (ce.hasCommand()) {
        Command c = ce.getCmd();
        if (c.equals("ls")) {
            Serial.println(ls_cmd_usage);
        } else if (c.equals("rm") || c.equals("del") ) {
            Serial.println(del_cmd_usage);
        } else if (c.equals("add")) {
            Serial.println(add_cmd_usage);
        } else if (c.equals("factory_reset")) {
            Serial.println(factory_cmd_usage);
        }
    } else {
        print_usage(NULL);
    }
}

void
print_usage(cmd *pcmd)
{
    (void)(pcmd);

    print_welcome(Serial);

    for (uint8_t i = 0; i < USAGE_SIZE; ++i) {
        Serial.println(usage[i]);
    }
}

void
add_account(cmd *pcmd)
{
    Command add_cmd(pcmd);

    Argument name_arg = add_cmd.getArgument(0);
    // if (!name_arg.isSet()) {
    //     Serial.println(F("FAIL: argument <name> is absent"));
    //     return;
    // }
    //
    Argument username_arg = add_cmd.getArgument(1);
    // if (!username_arg.isSet()) {
    //     Serial.println(F("FAIL: argument <username> is absent"));
    //     return;
    // }
    //
    Argument password_arg = add_cmd.getArgument(2);
    // // TODO: generate password if user did not pass it
    //
    if (modelStore.isExist(arg_get_value(name_arg.getPtr()))) {
        Serial.println(F("FAIL: Account with such name is already exist"));
        return;
    }

    Account acc;
    strncpy(acc.name, arg_get_value(name_arg.getPtr()), sizeof(acc.name));
    strncpy(acc.username, arg_get_value(username_arg.getPtr()), sizeof(acc.username));
    strncpy(acc.password, arg_get_value(password_arg.getPtr()), sizeof(acc.password));

    if (!modelStore.add(acc)) {
        Serial.println(F("FAIL: No space"));
    } else {
        Serial.println(F("Done"));
    }
}

void 
del_account(cmd *pcmd)
{
    Command del_cmd(pcmd);
    Argument name_arg = del_cmd.getArgument(0);
    if (!name_arg.isSet()) {
        Serial.println(F("FAIL: argument <name> is absent"));
    }

    if (!modelStore.del(arg_get_value(name_arg.getPtr()))) {
        Serial.println(F("FAIL"));
    } else {
        Serial.println(F("Done"));
    }
}

void
ls_accounts(__attribute__((unused)) cmd *pcmd) {
    Account acc;
    typename ModelStorage<Account>::ObjIndex idx = 0;
    if (!modelStore.get(idx, acc)) {
        Serial.println(F("Done"));
        return;
    }

    do
    {
        Serial.println(acc.name); 
        Serial.println(acc.username); 
        Serial.println(acc.password); 
    } while (modelStore.getNext(idx, acc, idx));

    Serial.println(F("Done"));
}

void
factory_reset(cmd *pcmd) {
    Command reset_cmd(pcmd);

    modelStore.factoryReset(); // TODO: how show progress?
    Serial.println(F("Done"));
}

void
cli_on()
{
    cli.unpause();
}

void
cli_off()
{
    cli.pause();
}
