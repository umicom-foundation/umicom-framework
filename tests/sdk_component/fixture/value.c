/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#include "sdk_fixture.h"

int sdk_composed_value(void) {
  /* Calling another archive exercises the exported transitive link interface. */
  return sdk_base_value() + 23;
}
