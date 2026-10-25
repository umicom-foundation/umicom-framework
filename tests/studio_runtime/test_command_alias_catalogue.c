/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_command_alias_catalogue.c
 *
 * PURPOSE:
 *   Focused regression coverage for Studio runtime command alias catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Required fixture setup and checks also execute in optimized builds. */
#include "../support/assertions.h"
#include "umicom/studio_runtime/command_alias_catalogue.h"
#include <assert.h>
#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
  size_t i, j;
/* The Kanban entry extends the supported catalogue. Retain the earlier
 * cardinality for compatibility review; exact identity checks remain active. */
#if 0
    assert(umi_studio_command_alias_count()==23U);
#endif
  /* The bookmark pane extends the exact catalogue. Keep the previous bound for review. */
#if 0
assert(umi_studio_command_alias_count() == 24U);
#endif
  assert(umi_studio_command_alias_count() == 25U);
  assert(umi_studio_command_alias_find("view.explorer") != NULL);
  assert(umi_studio_command_alias_find("designer.open") != NULL);
  /* Visit each bounded item once so every record receives the same rule. */
  for (i = 0U; i < umi_studio_command_alias_count(); ++i)
    /* Visit each bounded item once so every record receives the same rule. */
    for (j = i + 1U; j < umi_studio_command_alias_count(); ++j)
      assert(strcmp(umi_studio_command_alias_at(i)->alias_id,
                    umi_studio_command_alias_at(j)->alias_id) != 0);
  return 0;
}
