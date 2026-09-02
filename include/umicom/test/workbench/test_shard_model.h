/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_shard_model.h
 *
 * PURPOSE:
 *   Model test shard model state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_SHARD_MODEL_H
#define UMICOM_TEST_WORKBENCH_TEST_SHARD_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test shard model data shared with callers of this public contract.
 */
typedef struct UmiTestShardModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestShardModel;
/**
 * Initialise test shard model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_test_shard_model_init(UmiTestShardModel *model,const char *id,const char *label);
/**
 * Exercise test shard model set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_shard_model_set_active(UmiTestShardModel *model,bool active);
/**
 * Return the number of records represented by test shard model set without changing their
 * state.
 */
UmiStatus umi_test_shard_model_set_count(UmiTestShardModel *model,uint32_t item_count);
/**
 * Exercise test shard model set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_shard_model_set_state(UmiTestShardModel *model,UmiTestWorkbenchState state);
/**
 * Check that test shard model satisfies its contract before another service relies on it.
 */
int umi_test_shard_model_valid(const UmiTestShardModel *model);
#ifdef __cplusplus
}
#endif
#endif
