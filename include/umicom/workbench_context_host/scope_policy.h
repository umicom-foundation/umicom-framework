/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/scope_policy.h
 *
 * PURPOSE:
 *   Enforce workspace and application scope boundaries for cross-panel context propagation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_SCOPE_POLICY_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_SCOPE_POLICY_H
#include "umicom/workbench_context_host/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWorkbenchContextHostScopePolicy {
    char workspace_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char application_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    bool require_workspace_match;
    bool require_application_match;
    bool allow_empty_workspace;
    bool allow_cross_application;
} UmiWorkbenchContextHostScopePolicy;
void umi_workbench_context_host_scope_policy_init(UmiWorkbenchContextHostScopePolicy *policy);
bool umi_workbench_context_host_scope_allows(
    const UmiWorkbenchContextHostScopePolicy *policy,
    const UmiContextPayload *payload);
#ifdef __cplusplus
}
#endif
#endif
