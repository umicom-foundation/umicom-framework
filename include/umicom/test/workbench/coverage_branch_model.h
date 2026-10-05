/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/coverage_branch_model.h
 *
 * PURPOSE:
 *   Model coverage branch model state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_COVERAGE_BRANCH_MODEL_H
#define UMICOM_TEST_WORKBENCH_COVERAGE_BRANCH_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the coverage branch model data shared with callers of this public contract.
 */
typedef struct UmiCoverageBranchModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiCoverageBranchModel;
/**
 * Initialise coverage branch model from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_coverage_branch_model_init(UmiCoverageBranchModel *model,const char *id,const char *label);
/**
 * Exercise coverage branch model set active and return a clear result when the behaviour
 * no longer matches its contract.
 */
UmiStatus umi_coverage_branch_model_set_active(UmiCoverageBranchModel *model,bool active);
/**
 * Return the number of records represented by coverage branch model set without changing
 * their state.
 */
UmiStatus umi_coverage_branch_model_set_count(UmiCoverageBranchModel *model,uint32_t item_count);
/**
 * Exercise coverage branch model set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_coverage_branch_model_set_state(UmiCoverageBranchModel *model,UmiTestWorkbenchState state);
/**
 * Check that coverage branch model satisfies its contract before another service relies on
 * it.
 */
int umi_coverage_branch_model_valid(const UmiCoverageBranchModel *model);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_coverage_branch_model_archive_encode(const UmiCoverageBranchModel *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_coverage_branch_model_archive_decode(const void *bytes, size_t byte_count,
    UmiCoverageBranchModel *value);

#ifdef __cplusplus
}
#endif
#endif
