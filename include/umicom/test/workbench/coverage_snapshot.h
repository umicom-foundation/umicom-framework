/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/coverage_snapshot.h
 *
 * PURPOSE:
 *   Model coverage snapshot state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_COVERAGE_SNAPSHOT_H
#define UMICOM_TEST_WORKBENCH_COVERAGE_SNAPSHOT_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiCoverageSnapshot {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiCoverageSnapshot;
UmiStatus umi_coverage_snapshot_init(UmiCoverageSnapshot *model,const char *id,const char *label);
UmiStatus umi_coverage_snapshot_set_active(UmiCoverageSnapshot *model,bool active);
UmiStatus umi_coverage_snapshot_set_count(UmiCoverageSnapshot *model,uint32_t item_count);
UmiStatus umi_coverage_snapshot_set_state(UmiCoverageSnapshot *model,UmiTestWorkbenchState state);
int umi_coverage_snapshot_valid(const UmiCoverageSnapshot *model);
#ifdef __cplusplus
}
#endif
#endif
