/*
 * File: tests/sdk_component/fixture/late.c
 * Purpose: Provide a library registered after initial SDK composition.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 */
/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#include "sdk_fixture.h"

int sdk_deferred_value(void) {
  /* This artifact is registered after the initial SDK target is constructed. */
  return 7;
}
