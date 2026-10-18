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
/* Required fixture setup and checks also execute in optimized builds. */
#include "support/assertions.h"
#include "umicom/base/text.h"

#include <assert.h>
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* Editing has a stricter refusal rule than presentation copies. Verify all
 * bytes, overlapping ranges, exact fit and a one-byte destination explicitly. */
static int CheckTextReplacement(void)
{
  char text[8] = "abcdef";
  unsigned char before[sizeof(text)];
  memcpy(before, text, sizeof(text));
  if (umi_text_replace(text, sizeof(text), "12345678") != UMI_STATUS_CAPACITY_EXCEEDED ||
      memcmp(before, text, sizeof(text)) != 0) return 1;
  if (umi_text_replace(text, sizeof(text), NULL) != UMI_STATUS_INVALID_ARGUMENT ||
      memcmp(before, text, sizeof(text)) != 0) return 2;
  if (umi_text_replace(text, 0U, "x") != UMI_STATUS_INVALID_ARGUMENT ||
      memcmp(before, text, sizeof(text)) != 0) return 3;
  if (umi_text_replace(NULL, sizeof(text), "x") != UMI_STATUS_INVALID_ARGUMENT) return 4;
  if (umi_text_replace(text, sizeof(text), text + 2U) != UMI_STATUS_OK ||
      strcmp(text, "cdef") != 0) return 5;
  if (umi_text_replace(text + 1U, sizeof(text) - 1U, text) != UMI_STATUS_OK ||
      strcmp(text + 1U, "cdef") != 0) return 6;
  if (umi_text_replace(text, sizeof(text), "1234567") != UMI_STATUS_OK ||
      strcmp(text, "1234567") != 0) return 7;
  if (umi_text_replace(text, 1U, "") != UMI_STATUS_OK || text[0] != '\0') return 8;
  return 0;
}

int main(void) {
  if (CheckTextReplacement() != 0) return 1;
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
