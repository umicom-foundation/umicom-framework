/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_discovery_snapshot.h
 *
 * PURPOSE:
 *   Model test discovery snapshot state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_DISCOVERY_SNAPSHOT_H
#define UMICOM_TEST_WORKBENCH_TEST_DISCOVERY_SNAPSHOT_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test discovery snapshot data shared with callers of this public contract.
 */
typedef struct UmiTestDiscoverySnapshot {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestDiscoverySnapshot;
/**
 * Initialise test discovery snapshot from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_test_discovery_snapshot_init(UmiTestDiscoverySnapshot *model,const char *id,const char *label);
/**
 * Exercise test discovery snapshot set active and return a clear result when the behaviour
 * no longer matches its contract.
 */
UmiStatus umi_test_discovery_snapshot_set_active(UmiTestDiscoverySnapshot *model,bool active);
/**
 * Return the number of records represented by test discovery snapshot set without changing
 * their state.
 */
UmiStatus umi_test_discovery_snapshot_set_count(UmiTestDiscoverySnapshot *model,uint32_t item_count);
/**
 * Exercise test discovery snapshot set state and return a clear result when the behaviour
 * no longer matches its contract.
 */
UmiStatus umi_test_discovery_snapshot_set_state(UmiTestDiscoverySnapshot *model,UmiTestWorkbenchState state);
/**
 * Check that test discovery snapshot satisfies its contract before another service relies
 * on it.
 */
int umi_test_discovery_snapshot_valid(const UmiTestDiscoverySnapshot *model);
#ifdef __cplusplus
}
#endif
#endif
