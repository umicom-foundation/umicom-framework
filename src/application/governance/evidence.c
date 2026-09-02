/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/governance/evidence.c
 *
 * PURPOSE:
 *   Calculate component evidence gaps and safely add evidence or frontend
 *   support without accepting unknown governance flags.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/governance/evidence.h"

/*
 * Provide the component evidence known mask operation used by this module and its client
 * applications.
 */
uint32_t umi_component_evidence_known_mask(void) {
  return UMI_COMPONENT_EVIDENCE_CONTRACT | UMI_COMPONENT_EVIDENCE_IMPLEMENTATION |
         UMI_COMPONENT_EVIDENCE_UNIT_TEST | UMI_COMPONENT_EVIDENCE_HEADLESS |
         UMI_COMPONENT_EVIDENCE_GTK4 | UMI_COMPONENT_EVIDENCE_DOCUMENTATION |
         UMI_COMPONENT_EVIDENCE_ACCESSIBILITY | UMI_COMPONENT_EVIDENCE_CONSUMER;
}

/*
 * Provide the component frontend known mask operation used by this module and its client
 * applications.
 */
uint32_t umi_component_frontend_known_mask(void) {
  return UMI_COMPONENT_FRONTEND_HEADLESS | UMI_COMPONENT_FRONTEND_GTK4 |
         UMI_COMPONENT_FRONTEND_QT6 | UMI_COMPONENT_FRONTEND_WEB;
}

/*
 * Provide the component evidence required for status operation used by this module and its
 * client applications.
 */
uint32_t umi_component_evidence_required_for_status(UmiComponentApiStatus status) {
  const uint32_t planned = UMI_COMPONENT_EVIDENCE_CONTRACT | UMI_COMPONENT_EVIDENCE_DOCUMENTATION;
  const uint32_t experimental =
      planned | UMI_COMPONENT_EVIDENCE_IMPLEMENTATION | UMI_COMPONENT_EVIDENCE_UNIT_TEST;
  const uint32_t candidate = experimental | UMI_COMPONENT_EVIDENCE_HEADLESS |
                             UMI_COMPONENT_EVIDENCE_GTK4 | UMI_COMPONENT_EVIDENCE_ACCESSIBILITY |
                             UMI_COMPONENT_EVIDENCE_CONSUMER;

  /* Select the behaviour associated with the requested command or state value. */
  switch (status) {
  case UMI_COMPONENT_API_PLANNED:
    return planned;
  case UMI_COMPONENT_API_EXPERIMENTAL:
    return experimental;
  case UMI_COMPONENT_API_CANDIDATE:
  case UMI_COMPONENT_API_STABLE:
    return candidate;
  case UMI_COMPONENT_API_DEPRECATED:
    return UMI_COMPONENT_EVIDENCE_CONTRACT | UMI_COMPONENT_EVIDENCE_DOCUMENTATION;
  default:
    return 0U;
  }
}

/*
 * Provide the component frontends required for status operation used by this module and
 * its client applications.
 */
uint32_t umi_component_frontends_required_for_status(UmiComponentApiStatus status) {
  return status == UMI_COMPONENT_API_CANDIDATE || status == UMI_COMPONENT_API_STABLE
             ? UMI_COMPONENT_FRONTEND_HEADLESS | UMI_COMPONENT_FRONTEND_GTK4
             : 0U;
}

/*
 * Provide the component evidence missing operation used by this module and its client
 * applications.
 */
uint32_t umi_component_evidence_missing(const UmiComponentGovernanceRecord *record) {
  return record != NULL ? record->required_evidence & ~record->available_evidence : 0U;
}

/*
 * Provide the component frontends missing operation used by this module and its client
 * applications.
 */
uint32_t umi_component_frontends_missing(const UmiComponentGovernanceRecord *record) {
  uint32_t required;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (record == NULL)
    return 0U;
  required = umi_component_frontends_required_for_status(record->api_status);
  return required & ~record->frontend_support;
}

/*
 * Provide the component evidence is complete operation used by this module and its client
 * applications.
 */
int umi_component_evidence_is_complete(const UmiComponentGovernanceRecord *record) {
  return record != NULL && umi_component_evidence_missing(record) == 0U &&
         umi_component_frontends_missing(record) == 0U;
}

/* Return the number of records represented by component flag without changing their state. */
size_t umi_component_flag_count(uint32_t flags) {
  size_t count = 0U;

  /*
   * Continue only while work remains available; the loop body advances the state on each
   * pass.
   */
  while (flags != 0U) {
    count += (size_t)(flags & 1U);
    flags >>= 1U;
  }
  return count;
}

/* Add component evidence only after its inputs and available capacity have been checked. */
UmiStatus umi_component_evidence_add(UmiComponentGovernanceRecord *record, uint32_t evidence) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (record == NULL || evidence == 0U || (evidence & ~umi_component_evidence_known_mask()) != 0U) {
    return UMI_STATUS_INVALID_ARGUMENT;
  }
  record->available_evidence |= evidence;
  return UMI_STATUS_OK;
}

/* Add component frontend only after its inputs and available capacity have been checked. */
UmiStatus umi_component_frontend_add(UmiComponentGovernanceRecord *record, uint32_t frontend) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (record == NULL || frontend == 0U || (frontend & ~umi_component_frontend_known_mask()) != 0U) {
    return UMI_STATUS_INVALID_ARGUMENT;
  }
  record->frontend_support |= frontend;
  return UMI_STATUS_OK;
}
