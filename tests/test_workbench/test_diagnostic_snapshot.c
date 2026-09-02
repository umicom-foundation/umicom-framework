/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_diagnostic_snapshot.c
 *
 * PURPOSE:
 *   Verify diagnostic snapshot state, activity and item counts can be updated
 *   and validated through the reusable test workbench contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/diagnostic_snapshot.h"
#include <assert.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
  UmiTestWorkbenchDiagnosticSnapshot m;
  assert(umi_test_workbench_diagnostic_snapshot_init(
             &m, "diagnostic_snapshot", "Diagnostic Snapshot") ==
         UMI_STATUS_OK);
  assert(umi_test_workbench_diagnostic_snapshot_set_active(&m, true) ==
         UMI_STATUS_OK);
  assert(umi_test_workbench_diagnostic_snapshot_set_count(&m, 3U) ==
         UMI_STATUS_OK);
  assert(umi_test_workbench_diagnostic_snapshot_set_state(
             &m, UMI_TEST_WORKBENCH_STATE_READY) == UMI_STATUS_OK);
  assert(umi_test_workbench_diagnostic_snapshot_valid(&m));
  assert(m.active);
  assert(m.item_count == 3U);
  return 0;
}
