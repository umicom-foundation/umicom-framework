/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#include "sdk_fixture.h"

int sdk_deferred_value(void) {
  /* This artifact is registered after the initial SDK target is constructed. */
  return 7;
}
