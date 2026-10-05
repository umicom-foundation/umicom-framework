/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/flaky_test_model.h
 *
 * PURPOSE:
 *   Model flaky test model state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_FLAKY_TEST_MODEL_H
#define UMICOM_TEST_WORKBENCH_FLAKY_TEST_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the flaky test model data shared with callers of this public contract.
 */
typedef struct UmiFlakyTestModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiFlakyTestModel;
/**
 * Initialise flaky test model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_flaky_test_model_init(UmiFlakyTestModel *model,const char *id,const char *label);
/**
 * Exercise flaky test model set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_flaky_test_model_set_active(UmiFlakyTestModel *model,bool active);
/**
 * Return the number of records represented by flaky test model set without changing their
 * state.
 */
UmiStatus umi_flaky_test_model_set_count(UmiFlakyTestModel *model,uint32_t item_count);
/**
 * Exercise flaky test model set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_flaky_test_model_set_state(UmiFlakyTestModel *model,UmiTestWorkbenchState state);
/**
 * Check that flaky test model satisfies its contract before another service relies on it.
 */
int umi_flaky_test_model_valid(const UmiFlakyTestModel *model);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_flaky_test_model_archive_encode(const UmiFlakyTestModel *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_flaky_test_model_archive_decode(const void *bytes, size_t byte_count,
    UmiFlakyTestModel *value);

#ifdef __cplusplus
}
#endif
#endif
