/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/diagnostic_snapshot.h
 *
 * PURPOSE:
 *   Model diagnostic snapshot state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_DIAGNOSTIC_SNAPSHOT_H
#define UMICOM_TEST_WORKBENCH_DIAGNOSTIC_SNAPSHOT_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Keep this model namespace distinct from the core diagnostics snapshot ABI. */
typedef struct UmiTestWorkbenchDiagnosticSnapshot {
  UmiTestWorkbenchEntry value;
  uint64_t generation;
  uint32_t item_count;
  bool active;
} UmiTestWorkbenchDiagnosticSnapshot;
UmiStatus umi_test_workbench_diagnostic_snapshot_init(
    UmiTestWorkbenchDiagnosticSnapshot *model, const char *id,
    const char *label);
UmiStatus umi_test_workbench_diagnostic_snapshot_set_active(
    UmiTestWorkbenchDiagnosticSnapshot *model, bool active);
UmiStatus umi_test_workbench_diagnostic_snapshot_set_count(
    UmiTestWorkbenchDiagnosticSnapshot *model, uint32_t item_count);
UmiStatus umi_test_workbench_diagnostic_snapshot_set_state(
    UmiTestWorkbenchDiagnosticSnapshot *model, UmiTestWorkbenchState state);
int umi_test_workbench_diagnostic_snapshot_valid(
    const UmiTestWorkbenchDiagnosticSnapshot *model);
#ifdef __cplusplus
}
#endif
#endif
