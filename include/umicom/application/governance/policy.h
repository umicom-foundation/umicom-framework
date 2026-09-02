/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/governance/policy.h
 *
 * PURPOSE:
 *   Project default lifecycle policy over existing component definitions and
 *   evaluate explicit, evidence-backed promotion or deprecation decisions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_GOVERNANCE_POLICY_H
#define UMICOM_APPLICATION_GOVERNANCE_POLICY_H

#include "umicom/application/governance/evidence.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the component promotion assessment data shared with callers of this public
 * contract.
 */
typedef struct UmiComponentPromotionAssessment {
  UmiComponentApiStatus current_status;
  UmiComponentApiStatus target_status;
  uint32_t required_evidence;
  uint32_t missing_evidence;
  uint32_t required_frontends;
  uint32_t missing_frontends;
  int transition_allowed;
  int promotion_ready;
} UmiComponentPromotionAssessment;

/**
 * Provide the component governance default operation used by this module and its client
 * applications.
 */
UmiStatus umi_component_governance_default(const UmiApplicationComponentDefinition *definition,
                                           UmiComponentGovernanceRecord *out_record);
/**
 * Provide the component governance apply override operation used by this module and its
 * client applications.
 */
UmiStatus
umi_component_governance_apply_override(UmiComponentGovernanceRecord *record,
                                        const UmiComponentGovernanceOverride *override_record);
/**
 * Provide the component status transition allowed operation used by this module and its
 * client applications.
 */
int umi_component_status_transition_allowed(UmiComponentApiStatus current_status,
                                            UmiComponentApiStatus target_status);
/**
 * Provide the component promotion assess operation used by this module and its client
 * applications.
 */
UmiStatus umi_component_promotion_assess(const UmiComponentGovernanceRecord *record,
                                         UmiComponentApiStatus target_status,
                                         UmiComponentPromotionAssessment *out_assessment);

#ifdef __cplusplus
}
#endif

#endif
