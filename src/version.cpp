#include "version.hpp"

#include "creds_holder.hpp"
#include <Print.h>
#include <string.h>

void
print_welcome(Print& out)
{
  out.println(F(TITLE " v" VERSION));
  out.println(F("HW Creds manager"));
  out.println(F(AUTHOR));
  // out.println(F(EMAIL));

  out.println(__DATE__);
  // out.print(F(" "));
  out.println(__TIME__);
}
