/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_component_governance_release_gate.c
 *
 * PURPOSE:
 *   Verify candidate gaps block release, stable evidence can pass in isolation
 *   and deprecated components require an explicit replacement route.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/application/application.h"

int main(void) {
  UmiComponentInventory inventory;
  UmiComponentReleasePolicy policy;
  UmiComponentReleaseReport report;
  UmiComponentGovernanceOverride override_record = {0};

  assert(umi_component_inventory_build(NULL, 0U, &inventory) == UMI_STATUS_OK);
  umi_component_release_policy_init(&policy);
  assert(umi_component_release_gate_evaluate(&inventory, &policy, &report) == UMI_STATUS_OK);
  assert(!report.passed);
  assert(report.evaluated_component_count == inventory.status_count[UMI_COMPONENT_API_CANDIDATE]);
  assert(report.blocker_count > 100U);

  policy.status_mask = umi_component_api_status_mask(UMI_COMPONENT_API_STABLE);
  assert(umi_component_release_gate_evaluate(&inventory, &policy, &report) == UMI_STATUS_OK);
  assert(report.passed);

  override_record.component_id = "umicom.trading.chart";
  override_record.api_status = UMI_COMPONENT_API_STABLE;
  override_record.available_evidence = umi_component_evidence_known_mask();
  override_record.frontend_support = UMI_COMPONENT_FRONTEND_HEADLESS | UMI_COMPONENT_FRONTEND_GTK4;
  override_record.replace_available_evidence = 1;
  override_record.replace_frontend_support = 1;
  assert(umi_component_inventory_build(&override_record, 1U, &inventory) == UMI_STATUS_OK);
  assert(umi_component_release_gate_evaluate(&inventory, &policy, &report) == UMI_STATUS_OK);
  assert(report.passed);
  assert(report.evaluated_component_count == 1U);

  override_record.available_evidence &= ~(uint32_t)UMI_COMPONENT_EVIDENCE_ACCESSIBILITY;
  assert(umi_component_inventory_build(&override_record, 1U, &inventory) == UMI_STATUS_OK);
  assert(umi_component_release_gate_evaluate(&inventory, &policy, &report) == UMI_STATUS_OK);
  assert(!report.passed);
  assert(report.findings[0].missing_evidence != 0U);

  override_record.api_status = UMI_COMPONENT_API_DEPRECATED;
  override_record.replacement_component_id = NULL;
  policy.status_mask = 0U;
  assert(umi_component_inventory_build(&override_record, 1U, &inventory) == UMI_STATUS_OK);
  assert(umi_component_release_gate_evaluate(&inventory, &policy, &report) == UMI_STATUS_OK);
  assert(!report.passed);
  policy.status_mask = 1U << 30;
  assert(umi_component_release_gate_evaluate(&inventory, &policy, &report) ==
         UMI_STATUS_INVALID_ARGUMENT);
  return 0;
}
