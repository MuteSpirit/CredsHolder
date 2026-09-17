#if defined(EPOXY_DUINO)
#include <AUnitVerbose.h>

void
setup()
{
  Serial.setLineModeUnix(); // use Unix line end instead of DOS
}

void
loop()
{
  aunit::TestRunner::run();
}
#endif // EPOXY_DUINO
