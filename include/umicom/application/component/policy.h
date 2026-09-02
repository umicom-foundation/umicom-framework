/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/component/policy.h
 *
 * PURPOSE:
 *   Define guardrails for editable workspaces and evaluate a draft while
 *   returning useful diagnostics instead of one unexplained failure code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_COMPONENT_POLICY_H
#define UMICOM_APPLICATION_COMPONENT_POLICY_H

#include "umicom/application/component/diagnostic.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application component workspace policy data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationComponentWorkspacePolicy {
  size_t maximum_slots;
  uint32_t maximum_weight;
  int require_visible_component;
  int require_primary_component;
  int allow_floating_components;
} UmiApplicationComponentWorkspacePolicy;

/**
 * Provide the application component workspace policy default operation used by this module
 * and its client applications.
 */
void umi_application_component_workspace_policy_default(
    UmiApplicationComponentWorkspacePolicy *policy);
/**
 * Provide the application component workspace policy learning operation used by this
 * module and its client applications.
 */
void umi_application_component_workspace_policy_learning(
    UmiApplicationComponentWorkspacePolicy *policy);
/**
 * Provide the application component workspace policy evaluate operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_component_workspace_policy_evaluate(
    const UmiApplicationComponentRegistry *registry,
    const UmiApplicationComponentWorkspaceDraft *draft, UmiApplicationComponentFrontend frontend,
    const UmiApplicationComponentWorkspacePolicy *policy,
    UmiApplicationComponentDiagnosticReport *out_report);

#ifdef __cplusplus
}
#endif

#endif
