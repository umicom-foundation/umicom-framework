/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/policy.c
 *
 * PURPOSE:
 *   Evaluate editable workspace layouts against component contracts and clear
 *   usability guardrails while collecting every problem found in one pass.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/component/policy.h"

#include <string.h>

#include "umicom/base/text.h"

/* Provide the add diagnostic operation used by this module and its client applications. */
static UmiStatus add_diagnostic(UmiApplicationComponentDiagnosticReport *report,
                                UmiApplicationComponentDiagnosticSeverity severity,
                                UmiApplicationComponentDiagnosticCode code, size_t slot_index,
                                const char *component_id, const char *message) {
  UmiApplicationComponentDiagnostic *entry;
  UmiStatus status;
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (report->count >= UMI_APPLICATION_COMPONENT_DIAGNOSTIC_CAPACITY)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  entry = &report->entries[report->count];
  (void)memset(entry, 0, sizeof(*entry));
  entry->severity = severity;
  entry->code = code;
  entry->slot_index = slot_index;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (component_id != NULL)
    (void)umi_text_copy_truncated(entry->component_id, sizeof(entry->component_id), component_id);
  status = umi_text_copy(entry->message, sizeof(entry->message), message);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK)
    return status;
  report->count += 1U;
  /* Apply this branch only when its contract condition is satisfied. */
  if (severity == UMI_APPLICATION_COMPONENT_DIAGNOSTIC_INFORMATION)
    report->information_count += 1U;
  else /* Apply this branch only when its contract condition is satisfied. */ if (severity == UMI_APPLICATION_COMPONENT_DIAGNOSTIC_WARNING)
    report->warning_count += 1U;
  /* Use this fallback path when the earlier condition does not apply. */
  else
    report->error_count += 1U;
  return UMI_STATUS_OK;
}

/*
 * Provide the application component workspace policy default operation used by this module
 * and its client applications.
 */
void umi_application_component_workspace_policy_default(
    UmiApplicationComponentWorkspacePolicy *policy) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (policy != NULL) {
    policy->maximum_slots = UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY;
    policy->maximum_weight = 12U;
    policy->require_visible_component = 1;
    policy->require_primary_component = 1;
    policy->allow_floating_components = 1;
  }
}

/*
 * Provide the application component workspace policy learning operation used by this
 * module and its client applications.
 */
void umi_application_component_workspace_policy_learning(
    UmiApplicationComponentWorkspacePolicy *policy) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (policy != NULL) {
    umi_application_component_workspace_policy_default(policy);
    policy->maximum_slots = 8U;
    policy->maximum_weight = 6U;
    policy->allow_floating_components = 0;
  }
}

/* Provide the inspect slot operation used by this module and its client applications. */
static UmiStatus inspect_slot(const UmiApplicationComponentRegistry *registry,
                              const UmiApplicationComponentWorkspaceDraft *draft,
                              UmiApplicationComponentFrontend frontend,
                              const UmiApplicationComponentWorkspacePolicy *policy,
                              UmiApplicationComponentDiagnosticReport *report, size_t index) {
  const UmiApplicationComponentDraftSlot *slot = &draft->slots[index];
  const UmiApplicationComponentContract *contract;
  size_t previous;
  UmiStatus status = UMI_STATUS_OK;

  /* Apply this branch only when its contract condition is satisfied. */
  if (slot->weight == 0U || slot->weight > policy->maximum_weight)
    status = add_diagnostic(report, UMI_APPLICATION_COMPONENT_DIAGNOSTIC_ERROR,
                            UMI_APPLICATION_COMPONENT_DIAGNOSTIC_INVALID_WEIGHT, index,
                            slot->component_id,
                            "This component has an invalid size weight. Choose a value inside "
                            "the workspace policy limit.");
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK && !policy->allow_floating_components &&
      slot->region == UMI_APPLICATION_COMPONENT_REGION_FLOATING)
    status = add_diagnostic(
        report, UMI_APPLICATION_COMPONENT_DIAGNOSTIC_ERROR,
        UMI_APPLICATION_COMPONENT_DIAGNOSTIC_FLOATING_RESTRICTED, index, slot->component_id,
        "This workspace does not allow floating components. Dock it into a named region.");
  contract = umi_application_component_registry_find(registry, slot->component_id);
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (status == UMI_STATUS_OK && contract == NULL)
    status = add_diagnostic(
        report, UMI_APPLICATION_COMPONENT_DIAGNOSTIC_ERROR,
        UMI_APPLICATION_COMPONENT_DIAGNOSTIC_MISSING_COMPONENT, index, slot->component_id,
        "The component is not registered. Check its ID or install the feature that provides it.");
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (status == UMI_STATUS_OK && contract != NULL &&
      !umi_application_component_contract_supports(contract, frontend))
    status = add_diagnostic(report, UMI_APPLICATION_COMPONENT_DIAGNOSTIC_ERROR,
                            UMI_APPLICATION_COMPONENT_DIAGNOSTIC_UNSUPPORTED_FRONTEND, index,
                            slot->component_id,
                            "The selected frontend cannot display this component. Choose another "
                            "component or frontend.");
  /* Visit each bounded item once so every record receives the same rule. */
  for (previous = 0U; status == UMI_STATUS_OK && previous < index; ++previous) {
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(draft->slots[previous].instance_id, slot->instance_id) == 0)
      status = add_diagnostic(report, UMI_APPLICATION_COMPONENT_DIAGNOSTIC_ERROR,
                              UMI_APPLICATION_COMPONENT_DIAGNOSTIC_DUPLICATE_INSTANCE, index,
                              slot->component_id,
                              "Two components use the same instance ID. Give each visible copy "
                              "its own name.");
    else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (contract != NULL && !contract->definition->multi_instance &&
             strcmp(draft->slots[previous].component_id, slot->component_id) == 0)
      status = add_diagnostic(
          report, UMI_APPLICATION_COMPONENT_DIAGNOSTIC_ERROR,
          UMI_APPLICATION_COMPONENT_DIAGNOSTIC_DUPLICATE_SINGLETON, index, slot->component_id,
          "This component allows only one copy in a workspace. Remove the duplicate.");
  }
  return status;
}

/*
 * Provide the application component workspace policy evaluate operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_component_workspace_policy_evaluate(
    const UmiApplicationComponentRegistry *registry,
    const UmiApplicationComponentWorkspaceDraft *draft, UmiApplicationComponentFrontend frontend,
    const UmiApplicationComponentWorkspacePolicy *policy,
    UmiApplicationComponentDiagnosticReport *out_report) {
  size_t index;
  size_t visible_count = 0U;
  size_t primary_count = 0U;
  UmiStatus status = UMI_STATUS_OK;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (registry == NULL || draft == NULL || policy == NULL || out_report == NULL ||
      !umi_application_component_frontend_valid(frontend) || policy->maximum_slots == 0U ||
      policy->maximum_slots > UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY ||
      policy->maximum_weight == 0U || draft->slot_count > UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY)
    return UMI_STATUS_INVALID_ARGUMENT;
  umi_application_component_diagnostic_report_init(out_report);
  /* Apply this branch only when its contract condition is satisfied. */
  if (draft->slot_count > policy->maximum_slots)
    status = add_diagnostic(out_report, UMI_APPLICATION_COMPONENT_DIAGNOSTIC_ERROR,
                            UMI_APPLICATION_COMPONENT_DIAGNOSTIC_TOO_MANY_SLOTS, 0U, NULL,
                            "This workspace contains more components than its policy allows. "
                            "Remove optional components or use a larger policy.");
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; status == UMI_STATUS_OK && index < draft->slot_count; ++index) {
    visible_count += draft->slots[index].visible != 0;
    primary_count += draft->slots[index].visible &&
                     draft->slots[index].region == UMI_APPLICATION_COMPONENT_REGION_PRIMARY;
    status = inspect_slot(registry, draft, frontend, policy, out_report, index);
  }
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK && policy->require_visible_component && visible_count == 0U)
    status = add_diagnostic(out_report, UMI_APPLICATION_COMPONENT_DIAGNOSTIC_ERROR,
                            UMI_APPLICATION_COMPONENT_DIAGNOSTIC_NO_VISIBLE_COMPONENT, 0U, NULL,
                            "Every component is hidden. Make at least one component visible so "
                            "the workspace is useful.");
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK && policy->require_primary_component && primary_count == 0U)
    status = add_diagnostic(out_report, UMI_APPLICATION_COMPONENT_DIAGNOSTIC_ERROR,
                            UMI_APPLICATION_COMPONENT_DIAGNOSTIC_NO_PRIMARY_COMPONENT, 0U, NULL,
                            "The workspace has no visible primary component. Place its main "
                            "working surface in the primary region.");
  return status;
}
