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
#include "umicom/base/value_archive.h"
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
/**
 * Initialise test workbench diagnostic snapshot from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_test_workbench_diagnostic_snapshot_init(
    UmiTestWorkbenchDiagnosticSnapshot *model, const char *id,
    const char *label);
/**
 * Exercise test workbench diagnostic snapshot set active and return a clear result when
 * the behaviour no longer matches its contract.
 */
UmiStatus umi_test_workbench_diagnostic_snapshot_set_active(
    UmiTestWorkbenchDiagnosticSnapshot *model, bool active);
/**
 * Return the number of records represented by test workbench diagnostic snapshot set
 * without changing their state.
 */
UmiStatus umi_test_workbench_diagnostic_snapshot_set_count(
    UmiTestWorkbenchDiagnosticSnapshot *model, uint32_t item_count);
/**
 * Exercise test workbench diagnostic snapshot set state and return a clear result when the
 * behaviour no longer matches its contract.
 */
UmiStatus umi_test_workbench_diagnostic_snapshot_set_state(
    UmiTestWorkbenchDiagnosticSnapshot *model, UmiTestWorkbenchState state);
/**
 * Check that test workbench diagnostic snapshot satisfies its contract before another
 * service relies on it.
 */
int umi_test_workbench_diagnostic_snapshot_valid(
    const UmiTestWorkbenchDiagnosticSnapshot *model);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_test_workbench_diagnostic_snapshot_archive_encode(const UmiTestWorkbenchDiagnosticSnapshot *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_test_workbench_diagnostic_snapshot_archive_decode(const void *bytes, size_t byte_count,
    UmiTestWorkbenchDiagnosticSnapshot *value);

#ifdef __cplusplus
}
#endif
#endif
