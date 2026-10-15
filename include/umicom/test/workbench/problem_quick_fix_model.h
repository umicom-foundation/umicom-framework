/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/problem_quick_fix_model.h
 *
 * PURPOSE:
 *   Model problem quick fix model state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_PROBLEM_QUICK_FIX_MODEL_H
#define UMICOM_TEST_WORKBENCH_PROBLEM_QUICK_FIX_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the problem quick fix model data shared with callers of this public contract.
 */
typedef struct UmiProblemQuickFixModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiProblemQuickFixModel;
/**
 * Initialise problem quick fix model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_problem_quick_fix_model_init(UmiProblemQuickFixModel *model,const char *id,const char *label);
/**
 * Exercise problem quick fix model set active and return a clear result when the behaviour
 * no longer matches its contract.
 */
UmiStatus umi_problem_quick_fix_model_set_active(UmiProblemQuickFixModel *model,bool active);
/**
 * Return the number of records represented by problem quick fix model set without changing
 * their state.
 */
UmiStatus umi_problem_quick_fix_model_set_count(UmiProblemQuickFixModel *model,uint32_t item_count);
/**
 * Exercise problem quick fix model set state and return a clear result when the behaviour
 * no longer matches its contract.
 */
UmiStatus umi_problem_quick_fix_model_set_state(UmiProblemQuickFixModel *model,UmiTestWorkbenchState state);
/**
 * Check that problem quick fix model satisfies its contract before another service relies
 * on it.
 */
int umi_problem_quick_fix_model_valid(const UmiProblemQuickFixModel *model);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_problem_quick_fix_model_archive_encode(const UmiProblemQuickFixModel *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_problem_quick_fix_model_archive_decode(const void *bytes, size_t byte_count,
    UmiProblemQuickFixModel *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_problem_quick_fix_model_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiProblemQuickFixModel *expected, UmiProblemQuickFixModel *value);


#ifdef __cplusplus
}
#endif
#endif
