/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/coverage_snapshot.h
 *
 * PURPOSE:
 *   Model coverage snapshot state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_COVERAGE_SNAPSHOT_H
#define UMICOM_TEST_WORKBENCH_COVERAGE_SNAPSHOT_H
#include "umicom/test/workbench/workbench_types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the coverage snapshot data shared with callers of this public contract.
 */
typedef struct UmiCoverageSnapshot {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiCoverageSnapshot;
/**
 * Initialise coverage snapshot from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_coverage_snapshot_init(UmiCoverageSnapshot *model,const char *id,const char *label);
/**
 * Exercise coverage snapshot set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_coverage_snapshot_set_active(UmiCoverageSnapshot *model,bool active);
/**
 * Return the number of records represented by coverage snapshot set without changing their
 * state.
 */
UmiStatus umi_coverage_snapshot_set_count(UmiCoverageSnapshot *model,uint32_t item_count);
/**
 * Exercise coverage snapshot set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_coverage_snapshot_set_state(UmiCoverageSnapshot *model,UmiTestWorkbenchState state);
/**
 * Check that coverage snapshot satisfies its contract before another service relies on it.
 */
int umi_coverage_snapshot_valid(const UmiCoverageSnapshot *model);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_coverage_snapshot_archive_encode(const UmiCoverageSnapshot *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_coverage_snapshot_archive_decode(const void *bytes, size_t byte_count,
    UmiCoverageSnapshot *value);

#ifdef __cplusplus
}
#endif
#endif
