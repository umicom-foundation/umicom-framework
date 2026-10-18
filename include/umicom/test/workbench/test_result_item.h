/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_result_item.h
 *
 * PURPOSE:
 *   Model test result item state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_RESULT_ITEM_H
#define UMICOM_TEST_WORKBENCH_TEST_RESULT_ITEM_H
#include "umicom/test/workbench/workbench_types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test result item data shared with callers of this public contract.
 */
typedef struct UmiTestResultItem {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestResultItem;
/**
 * Initialise test result item from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_test_result_item_init(UmiTestResultItem *model,const char *id,const char *label);
/**
 * Exercise test result item set active and return a clear result when the behaviour no
 * longer matches its contract.

 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: generation, value.revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_test_result_item_set_active(UmiTestResultItem *model,bool active);
/**
 * Return the number of records represented by test result item set without changing their
 * state.

 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: generation, value.revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_test_result_item_set_count(UmiTestResultItem *model,uint32_t item_count);
/**
 * Exercise test result item set state and return a clear result when the behaviour no
 * longer matches its contract.

 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: generation, value.revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_test_result_item_set_state(UmiTestResultItem *model,UmiTestWorkbenchState state);
/**
 * Check that test result item satisfies its contract before another service relies on it.
 */
int umi_test_result_item_valid(const UmiTestResultItem *model);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_test_result_item_archive_encode(const UmiTestResultItem *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_test_result_item_archive_decode(const void *bytes, size_t byte_count,
    UmiTestResultItem *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiTestResultItem object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_test_result_item_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiTestResultItem *workspace, UmiTestResultItem *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_test_result_item_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiTestResultItem *expected, UmiTestResultItem *value);


#ifdef __cplusplus
}
#endif
#endif
