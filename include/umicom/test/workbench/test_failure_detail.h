/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_failure_detail.h
 *
 * PURPOSE:
 *   Model test failure detail state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_FAILURE_DETAIL_H
#define UMICOM_TEST_WORKBENCH_TEST_FAILURE_DETAIL_H
#include "umicom/test/workbench/workbench_types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test failure detail data shared with callers of this public contract.
 */
typedef struct UmiTestFailureDetail {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestFailureDetail;
/**
 * Initialise test failure detail from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_test_failure_detail_init(UmiTestFailureDetail *model,const char *id,const char *label);
/**
 * Exercise test failure detail set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_failure_detail_set_active(UmiTestFailureDetail *model,bool active);
/**
 * Return the number of records represented by test failure detail set without changing
 * their state.
 */
UmiStatus umi_test_failure_detail_set_count(UmiTestFailureDetail *model,uint32_t item_count);
/**
 * Exercise test failure detail set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_failure_detail_set_state(UmiTestFailureDetail *model,UmiTestWorkbenchState state);
/**
 * Check that test failure detail satisfies its contract before another service relies on
 * it.
 */
int umi_test_failure_detail_valid(const UmiTestFailureDetail *model);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_test_failure_detail_archive_encode(const UmiTestFailureDetail *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_test_failure_detail_archive_decode(const void *bytes, size_t byte_count,
    UmiTestFailureDetail *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_test_failure_detail_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiTestFailureDetail *expected, UmiTestFailureDetail *value);


#ifdef __cplusplus
}
#endif
#endif
