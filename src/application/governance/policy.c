/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/governance/policy.c
 *
 * PURPOSE:
 *   Apply conservative default governance to the complete component catalogue
 *   and require explicit evidence before a component can be promoted.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/governance/policy.h"

#include <string.h>

/*
 * Provide the status from maturity operation used by this module and its client
 * applications.
 */
static UmiComponentApiStatus status_from_maturity(UmiCapabilityMaturity maturity) {
  /* Select the behaviour associated with the requested command or state value. */
  switch (maturity) {
  case UMI_CAPABILITY_PLANNED:
    return UMI_COMPONENT_API_PLANNED;
  case UMI_CAPABILITY_FOUNDATION:
    return UMI_COMPONENT_API_EXPERIMENTAL;
  case UMI_CAPABILITY_IMPLEMENTED:
    return UMI_COMPONENT_API_CANDIDATE;
  default:
    return 0;
  }
}

/* Check that api status satisfies its contract before another service relies on it. */
static int api_status_valid(UmiComponentApiStatus status) {
  return status >= UMI_COMPONENT_API_PLANNED && status <= UMI_COMPONENT_API_DEPRECATED;
}

/* Check that owner satisfies its contract before another service relies on it. */
static int owner_valid(UmiComponentOwner owner) {
  return owner >= UMI_COMPONENT_OWNER_FRAMEWORK && owner <= UMI_COMPONENT_OWNER_EXTERNAL_ADAPTER;
}

/*
 * Provide the component governance default operation used by this module and its client
 * applications.
 */
UmiStatus umi_component_governance_default(const UmiApplicationComponentDefinition *definition,
                                           UmiComponentGovernanceRecord *out_record) {
  UmiComponentApiStatus status;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (out_record == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (umi_application_component_definition_validate(definition) != UMI_STATUS_OK) {
    return UMI_STATUS_INVALID_ARGUMENT;
  }
  status = status_from_maturity(definition->maturity);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (!api_status_valid(status))
    return UMI_STATUS_INVALID_STATE;

  (void)memset(out_record, 0, sizeof(*out_record));
  out_record->definition = definition;
  out_record->api_status = status;
  out_record->owner = UMI_COMPONENT_OWNER_FRAMEWORK;
  out_record->required_evidence = umi_component_evidence_required_for_status(status);
  out_record->available_evidence =
      UMI_COMPONENT_EVIDENCE_CONTRACT | UMI_COMPONENT_EVIDENCE_DOCUMENTATION;
  /* Apply this branch only when its contract condition is satisfied. */
  if (definition->maturity != UMI_CAPABILITY_PLANNED) {
    out_record->available_evidence |= UMI_COMPONENT_EVIDENCE_IMPLEMENTATION;
  }
  out_record->introduced_version = "0.9.0";
  return UMI_STATUS_OK;
}

/*
 * Provide the component governance apply override operation used by this module and its
 * client applications.
 */
UmiStatus
umi_component_governance_apply_override(UmiComponentGovernanceRecord *record,
                                        const UmiComponentGovernanceOverride *override_record) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (record == NULL || record->definition == NULL || override_record == NULL ||
      override_record->component_id == NULL ||
      strcmp(record->definition->component_id, override_record->component_id) != 0) {
    return UMI_STATUS_INVALID_ARGUMENT;
  }
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (override_record->api_status != 0 && !api_status_valid(override_record->api_status)) {
    return UMI_STATUS_INVALID_ARGUMENT;
  }
  /* Apply this operation only while the related capability or state is available. */
  if (override_record->owner != 0 && !owner_valid(override_record->owner)) {
    return UMI_STATUS_INVALID_ARGUMENT;
  }
  /* Apply this branch only when its contract condition is satisfied. */
  if ((override_record->available_evidence & ~umi_component_evidence_known_mask()) != 0U ||
      (override_record->frontend_support & ~umi_component_frontend_known_mask()) != 0U) {
    return UMI_STATUS_INVALID_ARGUMENT;
  }

  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (override_record->api_status != 0) {
    record->api_status = override_record->api_status;
    record->required_evidence = umi_component_evidence_required_for_status(record->api_status);
  }
  /* Apply this branch only when its contract condition is satisfied. */
  if (override_record->owner != 0)
    record->owner = override_record->owner;
  /* Apply this branch only when its contract condition is satisfied. */
  if (override_record->replace_available_evidence) {
    record->available_evidence = override_record->available_evidence;
  } /* Use this fallback path when the earlier condition does not apply. */ else {
    record->available_evidence |= override_record->available_evidence;
  }
  /* Apply this branch only when its contract condition is satisfied. */
  if (override_record->replace_frontend_support) {
    record->frontend_support = override_record->frontend_support;
  } /* Use this fallback path when the earlier condition does not apply. */ else {
    record->frontend_support |= override_record->frontend_support;
  }
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (override_record->introduced_version != NULL &&
      override_record->introduced_version[0] != '\0') {
    record->introduced_version = override_record->introduced_version;
  }
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (override_record->replacement_component_id != NULL) {
    record->replacement_component_id = override_record->replacement_component_id;
  }
  return UMI_STATUS_OK;
}

/*
 * Provide the component status transition allowed operation used by this module and its
 * client applications.
 */
int umi_component_status_transition_allowed(UmiComponentApiStatus current_status,
                                            UmiComponentApiStatus target_status) {
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (!api_status_valid(current_status) || !api_status_valid(target_status)) {
    return 0;
  }
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (current_status == UMI_COMPONENT_API_DEPRECATED) {
    return target_status == UMI_COMPONENT_API_DEPRECATED;
  }
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (target_status == UMI_COMPONENT_API_DEPRECATED)
    return 1;
  return target_status >= current_status;
}

/*
 * Provide the component promotion assess operation used by this module and its client
 * applications.
 */
UmiStatus umi_component_promotion_assess(const UmiComponentGovernanceRecord *record,
                                         UmiComponentApiStatus target_status,
                                         UmiComponentPromotionAssessment *out_assessment) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (record == NULL || record->definition == NULL || out_assessment == NULL ||
      !api_status_valid(target_status)) {
    return UMI_STATUS_INVALID_ARGUMENT;
  }

  (void)memset(out_assessment, 0, sizeof(*out_assessment));
  out_assessment->current_status = record->api_status;
  out_assessment->target_status = target_status;
  out_assessment->required_evidence = umi_component_evidence_required_for_status(target_status);
  out_assessment->required_frontends = umi_component_frontends_required_for_status(target_status);
  out_assessment->missing_evidence =
      out_assessment->required_evidence & ~record->available_evidence;
  out_assessment->missing_frontends =
      out_assessment->required_frontends & ~record->frontend_support;
  out_assessment->transition_allowed =
      umi_component_status_transition_allowed(record->api_status, target_status);
  out_assessment->promotion_ready = out_assessment->transition_allowed &&
                                    out_assessment->missing_evidence == 0U &&
                                    out_assessment->missing_frontends == 0U;
  return UMI_STATUS_OK;
}
