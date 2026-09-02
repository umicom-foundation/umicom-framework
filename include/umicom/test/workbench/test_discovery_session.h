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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_TEST_DISCOVERY_SESSION_H
#define UMICOM_TEST_WORKBENCH_TEST_DISCOVERY_SESSION_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test discovery session data shared with callers of this public contract.
 */
typedef struct UmiTestDiscoverySession {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestDiscoverySession;
/**
 * Initialise test discovery session from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_test_discovery_session_init(UmiTestDiscoverySession *model,const char *id,const char *label);
/**
 * Exercise test discovery session set active and return a clear result when the behaviour
 * no longer matches its contract.
 */
UmiStatus umi_test_discovery_session_set_active(UmiTestDiscoverySession *model,bool active);
/**
 * Return the number of records represented by test discovery session set without changing
 * their state.
 */
UmiStatus umi_test_discovery_session_set_count(UmiTestDiscoverySession *model,uint32_t item_count);
/**
 * Exercise test discovery session set state and return a clear result when the behaviour
 * no longer matches its contract.
 */
UmiStatus umi_test_discovery_session_set_state(UmiTestDiscoverySession *model,UmiTestWorkbenchState state);
/**
 * Check that test discovery session satisfies its contract before another service relies
 * on it.
 */
int umi_test_discovery_session_valid(const UmiTestDiscoverySession *model);
#ifdef __cplusplus
}
#endif
#endif
