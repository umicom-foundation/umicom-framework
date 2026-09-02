/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/quality_workbench_snapshot.h
 *
 * PURPOSE:
 *   Model quality workbench snapshot state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_QUALITY_WORKBENCH_SNAPSHOT_H
#define UMICOM_TEST_WORKBENCH_QUALITY_WORKBENCH_SNAPSHOT_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the quality workbench snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiQualityWorkbenchSnapshot {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiQualityWorkbenchSnapshot;
/**
 * Initialise quality workbench snapshot from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_quality_workbench_snapshot_init(UmiQualityWorkbenchSnapshot *model,const char *id,const char *label);
/**
 * Exercise quality workbench snapshot set active and return a clear result when the
 * behaviour no longer matches its contract.
 */
UmiStatus umi_quality_workbench_snapshot_set_active(UmiQualityWorkbenchSnapshot *model,bool active);
/**
 * Return the number of records represented by quality workbench snapshot set without
 * changing their state.
 */
UmiStatus umi_quality_workbench_snapshot_set_count(UmiQualityWorkbenchSnapshot *model,uint32_t item_count);
/**
 * Exercise quality workbench snapshot set state and return a clear result when the
 * behaviour no longer matches its contract.
 */
UmiStatus umi_quality_workbench_snapshot_set_state(UmiQualityWorkbenchSnapshot *model,UmiTestWorkbenchState state);
/**
 * Check that quality workbench snapshot satisfies its contract before another service
 * relies on it.
 */
int umi_quality_workbench_snapshot_valid(const UmiQualityWorkbenchSnapshot *model);
#ifdef __cplusplus
}
#endif
#endif
