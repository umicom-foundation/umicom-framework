/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_component_governance_policy.c
 *
 * PURPOSE:
 *   Verify conservative default lifecycle mapping, evidence-backed promotion,
 *   safe deprecation transitions and explicit governance overrides.
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

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
  UmiComponentGovernanceRecord record;
  UmiComponentPromotionAssessment assessment;
  UmiComponentGovernanceOverride override_record = {0};
  const UmiApplicationComponentDefinition *definition =
      umi_application_component_catalogue_find("umicom.trading.chart");

  assert(definition != NULL);
  assert(umi_component_governance_default(definition, &record) == UMI_STATUS_OK);
  assert(record.owner == UMI_COMPONENT_OWNER_FRAMEWORK);
  assert(record.api_status == UMI_COMPONENT_API_CANDIDATE);
  assert(umi_component_promotion_assess(&record, UMI_COMPONENT_API_STABLE, &assessment) ==
         UMI_STATUS_OK);
  assert(assessment.transition_allowed);
  assert(!assessment.promotion_ready);

  assert(umi_component_evidence_add(&record, assessment.missing_evidence) == UMI_STATUS_OK);
  assert(umi_component_frontend_add(&record, assessment.missing_frontends) == UMI_STATUS_OK);
  assert(umi_component_promotion_assess(&record, UMI_COMPONENT_API_STABLE, &assessment) ==
         UMI_STATUS_OK);
  assert(assessment.promotion_ready);
  assert(!umi_component_status_transition_allowed(UMI_COMPONENT_API_STABLE,
                                                  UMI_COMPONENT_API_EXPERIMENTAL));
  assert(umi_component_status_transition_allowed(UMI_COMPONENT_API_CANDIDATE,
                                                 UMI_COMPONENT_API_DEPRECATED));

  override_record.component_id = definition->component_id;
  override_record.api_status = UMI_COMPONENT_API_STABLE;
  override_record.owner = UMI_COMPONENT_OWNER_FRAMEWORK;
  override_record.available_evidence = umi_component_evidence_known_mask();
  override_record.frontend_support = UMI_COMPONENT_FRONTEND_HEADLESS | UMI_COMPONENT_FRONTEND_GTK4;
  override_record.introduced_version = "1.0.0";
  override_record.replace_available_evidence = 1;
  override_record.replace_frontend_support = 1;
  assert(umi_component_governance_apply_override(&record, &override_record) == UMI_STATUS_OK);
  assert(record.api_status == UMI_COMPONENT_API_STABLE);
  assert(record.available_evidence == umi_component_evidence_known_mask());
  assert(record.frontend_support ==
         (UMI_COMPONENT_FRONTEND_HEADLESS | UMI_COMPONENT_FRONTEND_GTK4));
  assert(record.introduced_version == override_record.introduced_version);
  return 0;
}
