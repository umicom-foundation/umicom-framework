/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_base_text.c
 *
 * PURPOSE:
 *   Verify checked, truncating, appending and formatting text operations retain
 *   deterministic fixed-capacity semantics across Framework modules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/base/text.h"

#include <assert.h>
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
  char text[8];

  assert(umi_text_copy(text, sizeof(text), "umicom") == UMI_STATUS_OK);
  assert(strcmp(text, "umicom") == 0);
  assert(umi_text_copy(text, sizeof(text), "component") ==
         UMI_STATUS_CAPACITY_EXCEEDED);
  assert(text[0] == '\0');
  assert(umi_text_copy_truncated(text, sizeof(text), "component") == 9U);
  assert(strcmp(text, "compone") == 0);
  assert(umi_text_copy(text, sizeof(text), "umi") == UMI_STATUS_OK);
  assert(umi_text_append(text, sizeof(text), "com") == UMI_STATUS_OK);
  assert(strcmp(text, "umicom") == 0);
  assert(umi_text_append(text, sizeof(text), "-framework") ==
         UMI_STATUS_CAPACITY_EXCEEDED);
  assert(strcmp(text, "umicom") == 0);
  assert(umi_text_format(text, sizeof(text), "%s-%u", "mb", 61U) ==
         UMI_STATUS_OK);
  assert(strcmp(text, "mb-61") == 0);
  return 0;
}
