/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_contract.c
 *
 * PURPOSE:
 *   Focused regression coverage for Studio runtime contract.
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
#include "umicom/studio_runtime/contract.h"
#include <assert.h>
#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
  const UmiStudioRuntimeContract *contract = umi_studio_runtime_contract();
  assert(contract != NULL);
  assert(strcmp(contract->application_id, "umicom.studio") == 0);
/* The Kanban entry extends the supported catalogue. Retain the earlier
 * cardinality for compatibility review; exact identity checks remain active. */
#if 0
    assert(contract->required_surface_count==27U);
#endif
  assert(contract->required_surface_count == 28U);
/* The Kanban entry extends the supported catalogue. Retain the earlier
 * cardinality for compatibility review; exact identity checks remain active. */
#if 0
    assert(contract->required_alias_count==23U);
#endif
  assert(contract->required_alias_count == 24U);
  assert(contract->required_studio_command_count == 17U);
  assert(contract->required_layout_count == 10U);
  return 0;
}
