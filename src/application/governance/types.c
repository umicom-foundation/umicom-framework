/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/governance/types.c
 *
 * PURPOSE:
 *   Provide deterministic text and mask projections for component governance,
 *   diagnostics, native reports and application-facing tooling.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/governance/types.h"

/*
 * Provide the component api status text operation used by this module and its client
 * applications.
 */
const char *umi_component_api_status_text(UmiComponentApiStatus status) {
  /* Select the behaviour associated with the requested command or state value. */
  switch (status) {
  case UMI_COMPONENT_API_PLANNED:
    return "planned";
  case UMI_COMPONENT_API_EXPERIMENTAL:
    return "experimental";
  case UMI_COMPONENT_API_CANDIDATE:
    return "candidate";
  case UMI_COMPONENT_API_STABLE:
    return "stable";
  case UMI_COMPONENT_API_DEPRECATED:
    return "deprecated";
  default:
    return "unknown";
  }
}

/*
 * Provide the component owner text operation used by this module and its client
 * applications.
 */
const char *umi_component_owner_text(UmiComponentOwner owner) {
  /* Select the behaviour associated with the requested command or state value. */
  switch (owner) {
  case UMI_COMPONENT_OWNER_FRAMEWORK:
    return "framework";
  case UMI_COMPONENT_OWNER_APPLICATION:
    return "application";
  case UMI_COMPONENT_OWNER_EXTERNAL_ADAPTER:
    return "external-adapter";
  default:
    return "unknown";
  }
}

/*
 * Provide the component evidence text operation used by this module and its client
 * applications.
 */
const char *umi_component_evidence_text(uint32_t evidence) {
  /* Select the behaviour associated with the requested command or state value. */
  switch (evidence) {
  case UMI_COMPONENT_EVIDENCE_CONTRACT:
    return "contract";
  case UMI_COMPONENT_EVIDENCE_IMPLEMENTATION:
    return "implementation";
  case UMI_COMPONENT_EVIDENCE_UNIT_TEST:
    return "unit-test";
  case UMI_COMPONENT_EVIDENCE_HEADLESS:
    return "headless";
  case UMI_COMPONENT_EVIDENCE_GTK4:
    return "gtk4";
  case UMI_COMPONENT_EVIDENCE_DOCUMENTATION:
    return "documentation";
  case UMI_COMPONENT_EVIDENCE_ACCESSIBILITY:
    return "accessibility";
  case UMI_COMPONENT_EVIDENCE_CONSUMER:
    return "consumer";
  default:
    return "unknown";
  }
}

/*
 * Provide the component frontend text operation used by this module and its client
 * applications.
 */
const char *umi_component_frontend_text(uint32_t frontend) {
  /* Select the behaviour associated with the requested command or state value. */
  switch (frontend) {
  case UMI_COMPONENT_FRONTEND_HEADLESS:
    return "headless";
  case UMI_COMPONENT_FRONTEND_GTK4:
    return "gtk4";
  case UMI_COMPONENT_FRONTEND_QT6:
    return "qt6";
  case UMI_COMPONENT_FRONTEND_WEB:
    return "web";
  default:
    return "unknown";
  }
}

/*
 * Provide the component api status mask operation used by this module and its client
 * applications.
 */
uint32_t umi_component_api_status_mask(UmiComponentApiStatus status) {
  return status >= UMI_COMPONENT_API_PLANNED && status <= UMI_COMPONENT_API_DEPRECATED
             ? 1U << (uint32_t)status
             : 0U;
}

/*
 * Provide the component role mask operation used by this module and its client
 * applications.
 */
uint32_t umi_component_role_mask(UmiApplicationComponentRole role) {
  return role >= UMI_APPLICATION_COMPONENT_VIEW && role <= UMI_APPLICATION_COMPONENT_SERVICE_SURFACE
             ? 1U << (uint32_t)role
             : 0U;
}

/*
 * Provide the component maturity mask operation used by this module and its client
 * applications.
 */
uint32_t umi_component_maturity_mask(UmiCapabilityMaturity maturity) {
  return maturity >= UMI_CAPABILITY_IMPLEMENTED && maturity <= UMI_CAPABILITY_PLANNED
             ? 1U << (uint32_t)maturity
             : 0U;
}
