/*
 * File: tests/sdk_component/fixture/base.c
 * Purpose: Provide the base archive used by SDK installation fixtures.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 */
/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#include "sdk_fixture.h"

int sdk_base_value(void) {
  /* Stable nonzero evidence must come from the installed library. */
  return 19;
}
