/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/problem_navigation_model.h
 *
 * PURPOSE:
 *   Model problem navigation model state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_PROBLEM_NAVIGATION_MODEL_H
#define UMICOM_TEST_WORKBENCH_PROBLEM_NAVIGATION_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the problem navigation model data shared with callers of this public contract.
 */
typedef struct UmiProblemNavigationModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiProblemNavigationModel;
/**
 * Initialise problem navigation model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_problem_navigation_model_init(UmiProblemNavigationModel *model,const char *id,const char *label);
/**
 * Exercise problem navigation model set active and return a clear result when the
 * behaviour no longer matches its contract.
 */
UmiStatus umi_problem_navigation_model_set_active(UmiProblemNavigationModel *model,bool active);
/**
 * Return the number of records represented by problem navigation model set without
 * changing their state.
 */
UmiStatus umi_problem_navigation_model_set_count(UmiProblemNavigationModel *model,uint32_t item_count);
/**
 * Exercise problem navigation model set state and return a clear result when the behaviour
 * no longer matches its contract.
 */
UmiStatus umi_problem_navigation_model_set_state(UmiProblemNavigationModel *model,UmiTestWorkbenchState state);
/**
 * Check that problem navigation model satisfies its contract before another service relies
 * on it.
 */
int umi_problem_navigation_model_valid(const UmiProblemNavigationModel *model);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_problem_navigation_model_archive_encode(const UmiProblemNavigationModel *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_problem_navigation_model_archive_decode(const void *bytes, size_t byte_count,
    UmiProblemNavigationModel *value);

#ifdef __cplusplus
}
#endif
#endif
