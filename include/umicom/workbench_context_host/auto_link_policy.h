/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/auto_link_policy.h
 *
 * PURPOSE:
 *   Resolve recommended default colour groups from panel roles and context kinds.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_AUTO_LINK_POLICY_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_AUTO_LINK_POLICY_H
#include "umicom/workbench_context_host/profile.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWorkbenchContextHostAutoLinkDecision {
    char group_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    UmiWorkbenchContextLinkMode mode;
    int confidence;
    bool found;
} UmiWorkbenchContextHostAutoLinkDecision;
UmiStatus umi_workbench_context_host_auto_link_decide(
    const UmiWorkbenchContextHostProfile *profile,
    UmiWorkbenchContextHostPanelRole role,
    UmiContextKind kind,
    UmiWorkbenchContextHostAutoLinkDecision *out_decision);
#ifdef __cplusplus
}
#endif
#endif
