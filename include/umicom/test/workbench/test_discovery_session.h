/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_discovery_session.h
 *
 * PURPOSE:
 *   Model test discovery session state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_TEST_DISCOVERY_SESSION_H
#define UMICOM_TEST_WORKBENCH_TEST_DISCOVERY_SESSION_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestDiscoverySession {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestDiscoverySession;
UmiStatus umi_test_discovery_session_init(UmiTestDiscoverySession *model,const char *id,const char *label);
UmiStatus umi_test_discovery_session_set_active(UmiTestDiscoverySession *model,bool active);
UmiStatus umi_test_discovery_session_set_count(UmiTestDiscoverySession *model,uint32_t item_count);
UmiStatus umi_test_discovery_session_set_state(UmiTestDiscoverySession *model,UmiTestWorkbenchState state);
int umi_test_discovery_session_valid(const UmiTestDiscoverySession *model);
#ifdef __cplusplus
}
#endif
#endif
