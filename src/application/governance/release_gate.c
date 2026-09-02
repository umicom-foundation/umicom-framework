/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/governance/release_gate.c
 *
 * PURPOSE:
 *   Produce bounded, component-specific release findings so incomplete
 *   evidence cannot be mistaken for stable Framework behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/governance/release_gate.h"

#include <stdio.h>
#include <string.h>

/* Provide the add finding operation used by this module and its client applications. */
static void add_finding(UmiComponentReleaseReport *report, UmiComponentGateSeverity severity,
                        const char *rule_id, const char *component_id, const char *message,
                        uint32_t missing_evidence, uint32_t missing_frontends) {
  UmiComponentGateFinding *finding;

  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (report->finding_count >= UMI_COMPONENT_GOVERNANCE_MAX_FINDINGS) {
    report->truncated = 1;
    return;
  }
  finding = &report->findings[report->finding_count++];
  finding->severity = severity;
  finding->missing_evidence = missing_evidence;
  finding->missing_frontends = missing_frontends;
  (void)snprintf(finding->rule_id, sizeof(finding->rule_id), "%s", rule_id);
  (void)snprintf(finding->component_id, sizeof(finding->component_id), "%s", component_id);
  (void)snprintf(finding->message, sizeof(finding->message), "%s", message);
  /* Apply this branch only when its contract condition is satisfied. */
  if (severity == UMI_COMPONENT_GATE_BLOCKER)
    report->blocker_count += 1U;
  else /* Apply this branch only when its contract condition is satisfied. */ if (severity == UMI_COMPONENT_GATE_WARNING)
    report->warning_count += 1U;
  /* Use this fallback path when the earlier condition does not apply. */
  else
    report->information_count += 1U;
}

/*
 * Initialise component release policy from caller-provided values so later operations
 * receive a known state.
 */
void umi_component_release_policy_init(UmiComponentReleasePolicy *policy) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (policy == NULL)
    return;
  (void)memset(policy, 0, sizeof(*policy));
  policy->status_mask = umi_component_api_status_mask(UMI_COMPONENT_API_CANDIDATE) |
                        umi_component_api_status_mask(UMI_COMPONENT_API_STABLE);
  policy->require_framework_owner = 1;
  policy->require_deprecation_replacement = 1;
}

/*
 * Provide the component release gate evaluate operation used by this module and its client
 * applications.
 */
UmiStatus umi_component_release_gate_evaluate(const UmiComponentInventory *inventory,
                                              const UmiComponentReleasePolicy *policy,
                                              UmiComponentReleaseReport *out_report) {
  size_t index;
  const uint32_t known_status_mask = umi_component_api_status_mask(UMI_COMPONENT_API_PLANNED) |
                                     umi_component_api_status_mask(UMI_COMPONENT_API_EXPERIMENTAL) |
                                     umi_component_api_status_mask(UMI_COMPONENT_API_CANDIDATE) |
                                     umi_component_api_status_mask(UMI_COMPONENT_API_STABLE) |
                                     umi_component_api_status_mask(UMI_COMPONENT_API_DEPRECATED);

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (inventory == NULL || policy == NULL || out_report == NULL ||
      umi_component_inventory_validate(inventory) != UMI_STATUS_OK ||
      (policy->status_mask & ~known_status_mask) != 0U ||
      (policy->required_evidence & ~umi_component_evidence_known_mask()) != 0U ||
      (policy->required_frontends & ~umi_component_frontend_known_mask()) != 0U) {
    return UMI_STATUS_INVALID_ARGUMENT;
  }

  (void)memset(out_report, 0, sizeof(*out_report));
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < inventory->component_count; ++index) {
    const UmiComponentGovernanceRecord *record = &inventory->records[index];
    const char *component_id = record->definition->component_id;
    const uint32_t status_bit = umi_component_api_status_mask(record->api_status);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (record->api_status == UMI_COMPONENT_API_DEPRECATED &&
        policy->require_deprecation_replacement &&
        (record->replacement_component_id == NULL || record->replacement_component_id[0] == '\0')) {
      add_finding(out_report, UMI_COMPONENT_GATE_BLOCKER, "UMI-COMP-005", component_id,
                  "Deprecated component has no replacement route.", 0U, 0U);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((policy->status_mask & status_bit) == 0U)
      continue;
    out_report->evaluated_component_count += 1U;

    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->require_framework_owner && record->owner != UMI_COMPONENT_OWNER_FRAMEWORK) {
      add_finding(out_report, UMI_COMPONENT_GATE_BLOCKER, "UMI-COMP-002", component_id,
                  "Reusable catalogue component is not Framework-owned.", 0U, 0U);
    }
    {
      const uint32_t required_evidence = record->required_evidence | policy->required_evidence;
      const uint32_t missing_evidence = required_evidence & ~record->available_evidence;
      /* Apply this branch only when its contract condition is satisfied. */
      if (missing_evidence != 0U) {
        add_finding(out_report, UMI_COMPONENT_GATE_BLOCKER, "UMI-COMP-003", component_id,
                    "Component is missing required release evidence.", missing_evidence, 0U);
      }
    }
    {
      const uint32_t required_frontends =
          umi_component_frontends_required_for_status(record->api_status) |
          policy->required_frontends;
      const uint32_t missing_frontends = required_frontends & ~record->frontend_support;
      /* Apply this branch only when its contract condition is satisfied. */
      if (missing_frontends != 0U) {
        add_finding(out_report, UMI_COMPONENT_GATE_BLOCKER, "UMI-COMP-004", component_id,
                    "Component is missing required frontend conformance.", 0U, missing_frontends);
      }
    }
  }
  /* Apply this branch only when its contract condition is satisfied. */
  if (out_report->blocker_count == 0U) {
    add_finding(out_report, UMI_COMPONENT_GATE_INFORMATION, "UMI-COMP-000", "component-catalogue",
                "All release-scoped components satisfy governance policy.", 0U, 0U);
  }
  out_report->passed = out_report->blocker_count == 0U && !out_report->truncated;
  return UMI_STATUS_OK;
}

/*
 * Provide the component gate severity text operation used by this module and its client
 * applications.
 */
const char *umi_component_gate_severity_text(UmiComponentGateSeverity severity) {
  /* Select the behaviour associated with the requested command or state value. */
  switch (severity) {
  case UMI_COMPONENT_GATE_INFORMATION:
    return "information";
  case UMI_COMPONENT_GATE_WARNING:
    return "warning";
  case UMI_COMPONENT_GATE_BLOCKER:
    return "blocker";
  default:
    return "unknown";
  }
}
