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

typedef struct UmiApplicationComponentWorkspacePolicy {
  size_t maximum_slots;
  uint32_t maximum_weight;
  int require_visible_component;
  int require_primary_component;
  int allow_floating_components;
} UmiApplicationComponentWorkspacePolicy;

void umi_application_component_workspace_policy_default(
    UmiApplicationComponentWorkspacePolicy *policy);
void umi_application_component_workspace_policy_learning(
    UmiApplicationComponentWorkspacePolicy *policy);
UmiStatus umi_application_component_workspace_policy_evaluate(
    const UmiApplicationComponentRegistry *registry,
    const UmiApplicationComponentWorkspaceDraft *draft, UmiApplicationComponentFrontend frontend,
    const UmiApplicationComponentWorkspacePolicy *policy,
    UmiApplicationComponentDiagnosticReport *out_report);

#ifdef __cplusplus
}
#endif

#endif
