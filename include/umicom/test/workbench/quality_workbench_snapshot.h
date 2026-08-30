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
typedef struct UmiQualityWorkbenchSnapshot {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiQualityWorkbenchSnapshot;
UmiStatus umi_quality_workbench_snapshot_init(UmiQualityWorkbenchSnapshot *model,const char *id,const char *label);
UmiStatus umi_quality_workbench_snapshot_set_active(UmiQualityWorkbenchSnapshot *model,bool active);
UmiStatus umi_quality_workbench_snapshot_set_count(UmiQualityWorkbenchSnapshot *model,uint32_t item_count);
UmiStatus umi_quality_workbench_snapshot_set_state(UmiQualityWorkbenchSnapshot *model,UmiTestWorkbenchState state);
int umi_quality_workbench_snapshot_valid(const UmiQualityWorkbenchSnapshot *model);
#ifdef __cplusplus
}
#endif
#endif
