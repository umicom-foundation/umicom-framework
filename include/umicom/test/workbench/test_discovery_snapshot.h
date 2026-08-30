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
typedef struct UmiTestDiscoverySnapshot {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestDiscoverySnapshot;
UmiStatus umi_test_discovery_snapshot_init(UmiTestDiscoverySnapshot *model,const char *id,const char *label);
UmiStatus umi_test_discovery_snapshot_set_active(UmiTestDiscoverySnapshot *model,bool active);
UmiStatus umi_test_discovery_snapshot_set_count(UmiTestDiscoverySnapshot *model,uint32_t item_count);
UmiStatus umi_test_discovery_snapshot_set_state(UmiTestDiscoverySnapshot *model,UmiTestWorkbenchState state);
int umi_test_discovery_snapshot_valid(const UmiTestDiscoverySnapshot *model);
#ifdef __cplusplus
}
#endif
#endif
