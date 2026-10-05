/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_impact_summary.h
 *
 * PURPOSE:
 *   Model test impact summary state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_IMPACT_SUMMARY_H
#define UMICOM_TEST_WORKBENCH_TEST_IMPACT_SUMMARY_H
#include "umicom/test/workbench/workbench_types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test impact summary data shared with callers of this public contract.
 */
typedef struct UmiTestImpactSummary {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestImpactSummary;
/**
 * Initialise test impact summary from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_test_impact_summary_init(UmiTestImpactSummary *model,const char *id,const char *label);
/**
 * Exercise test impact summary set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_impact_summary_set_active(UmiTestImpactSummary *model,bool active);
/**
 * Return the number of records represented by test impact summary set without changing
 * their state.
 */
UmiStatus umi_test_impact_summary_set_count(UmiTestImpactSummary *model,uint32_t item_count);
/**
 * Exercise test impact summary set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_impact_summary_set_state(UmiTestImpactSummary *model,UmiTestWorkbenchState state);
/**
 * Check that test impact summary satisfies its contract before another service relies on
 * it.
 */
int umi_test_impact_summary_valid(const UmiTestImpactSummary *model);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_test_impact_summary_archive_encode(const UmiTestImpactSummary *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_test_impact_summary_archive_decode(const void *bytes, size_t byte_count,
    UmiTestImpactSummary *value);

#ifdef __cplusplus
}
#endif
#endif
