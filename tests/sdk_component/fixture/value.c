/*
 * File: tests/sdk_component/fixture/value.c
 * Purpose: Exercise dependencies between installed SDK archives.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 */
/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#include "sdk_fixture.h"

int sdk_composed_value(void) {
  /* Calling another archive exercises the exported transitive link interface. */
  return sdk_base_value() + 23;
}
