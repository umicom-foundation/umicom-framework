/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_discovery_state.h
 *
 * PURPOSE:
 *   Model test discovery state state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_TEST_DISCOVERY_STATE_H
#define UMICOM_TEST_WORKBENCH_TEST_DISCOVERY_STATE_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestDiscoveryState {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestDiscoveryState;
UmiStatus umi_test_discovery_state_init(UmiTestDiscoveryState *model,const char *id,const char *label);
UmiStatus umi_test_discovery_state_set_active(UmiTestDiscoveryState *model,bool active);
UmiStatus umi_test_discovery_state_set_count(UmiTestDiscoveryState *model,uint32_t item_count);
UmiStatus umi_test_discovery_state_set_state(UmiTestDiscoveryState *model,UmiTestWorkbenchState state);
int umi_test_discovery_state_valid(const UmiTestDiscoveryState *model);
#ifdef __cplusplus
}
#endif
#endif
