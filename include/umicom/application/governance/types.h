/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/governance/types.h
 *
 * PURPOSE:
 *   Define stable component-governance identities, lifecycle states, evidence
 *   flags and frontend support metadata without changing existing catalogue
 *   records or exposing toolkit-specific types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_GOVERNANCE_TYPES_H
#define UMICOM_APPLICATION_GOVERNANCE_TYPES_H

#include <stdint.h>

#include "umicom/application/component_catalogue.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_COMPONENT_GOVERNANCE_MAX_COMPONENTS 256U
#define UMI_COMPONENT_GOVERNANCE_MAX_DOMAINS 64U
#define UMI_COMPONENT_GOVERNANCE_MAX_FINDINGS 512U
#define UMI_COMPONENT_GOVERNANCE_TEXT_CAPACITY 256U

/**
 * List the named component api status values accepted by this public contract.
 */
typedef enum UmiComponentApiStatus {
  UMI_COMPONENT_API_PLANNED = 1,
  UMI_COMPONENT_API_EXPERIMENTAL = 2,
  UMI_COMPONENT_API_CANDIDATE = 3,
  UMI_COMPONENT_API_STABLE = 4,
  UMI_COMPONENT_API_DEPRECATED = 5
} UmiComponentApiStatus;

/**
 * List the named component owner values accepted by this public contract.
 */
typedef enum UmiComponentOwner {
  UMI_COMPONENT_OWNER_FRAMEWORK = 1,
  UMI_COMPONENT_OWNER_APPLICATION = 2,
  UMI_COMPONENT_OWNER_EXTERNAL_ADAPTER = 3
} UmiComponentOwner;

/**
 * List the named component evidence values accepted by this public contract.
 */
typedef enum UmiComponentEvidence {
  UMI_COMPONENT_EVIDENCE_CONTRACT = 1U << 0,
  UMI_COMPONENT_EVIDENCE_IMPLEMENTATION = 1U << 1,
  UMI_COMPONENT_EVIDENCE_UNIT_TEST = 1U << 2,
  UMI_COMPONENT_EVIDENCE_HEADLESS = 1U << 3,
  UMI_COMPONENT_EVIDENCE_GTK4 = 1U << 4,
  UMI_COMPONENT_EVIDENCE_DOCUMENTATION = 1U << 5,
  UMI_COMPONENT_EVIDENCE_ACCESSIBILITY = 1U << 6,
  UMI_COMPONENT_EVIDENCE_CONSUMER = 1U << 7
} UmiComponentEvidence;

/**
 * List the named component frontend values accepted by this public contract.
 */
typedef enum UmiComponentFrontend {
  UMI_COMPONENT_FRONTEND_HEADLESS = 1U << 0,
  UMI_COMPONENT_FRONTEND_GTK4 = 1U << 1,
  UMI_COMPONENT_FRONTEND_QT6 = 1U << 2,
  UMI_COMPONENT_FRONTEND_WEB = 1U << 3
} UmiComponentFrontend;

/**
 * Represent the component governance record data shared with callers of this public
 * contract.
 */
typedef struct UmiComponentGovernanceRecord {
  const UmiApplicationComponentDefinition *definition;
  UmiComponentApiStatus api_status;
  UmiComponentOwner owner;
  uint32_t required_evidence;
  uint32_t available_evidence;
  uint32_t frontend_support;
  const char *introduced_version;
  const char *replacement_component_id;
} UmiComponentGovernanceRecord;

/**
 * Represent the component governance override data shared with callers of this public
 * contract.
 */
typedef struct UmiComponentGovernanceOverride {
  const char *component_id;
  UmiComponentApiStatus api_status;
  UmiComponentOwner owner;
  uint32_t available_evidence;
  uint32_t frontend_support;
  const char *introduced_version;
  const char *replacement_component_id;
  int replace_available_evidence;
  int replace_frontend_support;
} UmiComponentGovernanceOverride;

/**
 * Provide the component api status text operation used by this module and its client
 * applications.
 */
const char *umi_component_api_status_text(UmiComponentApiStatus status);
/**
 * Provide the component owner text operation used by this module and its client
 * applications.
 */
const char *umi_component_owner_text(UmiComponentOwner owner);
/**
 * Provide the component evidence text operation used by this module and its client
 * applications.
 */
const char *umi_component_evidence_text(uint32_t evidence);
/**
 * Provide the component frontend text operation used by this module and its client
 * applications.
 */
const char *umi_component_frontend_text(uint32_t frontend);
/**
 * Provide the component api status mask operation used by this module and its client
 * applications.
 */
uint32_t umi_component_api_status_mask(UmiComponentApiStatus status);
/**
 * Provide the component role mask operation used by this module and its client
 * applications.
 */
uint32_t umi_component_role_mask(UmiApplicationComponentRole role);
/**
 * Provide the component maturity mask operation used by this module and its client
 * applications.
 */
uint32_t umi_component_maturity_mask(UmiCapabilityMaturity maturity);

#ifdef __cplusplus
}
#endif

#endif
