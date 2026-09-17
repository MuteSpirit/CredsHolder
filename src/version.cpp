#include "version.hpp"

#if defined(ARDUINO_ARCH_NRF52)
#define NRF52840
#  if !defined(USE_TINYUSB)
#  define USE_TINYUSB 1
#  endif
#endif
#include <Print.h>

void
print_welcome(Print& out)
{
  out.println(F(TITLE " v" VERSION));
  out.println(F("HW Creds manager"));
  out.println(F(AUTHOR));
  out.println(F(EMAIL));

  out.print(__DATE__);
  out.print(F(" "));
  out.println(__TIME__);
}
