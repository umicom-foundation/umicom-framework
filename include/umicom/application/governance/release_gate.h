/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/governance/release_gate.h
 *
 * PURPOSE:
 *   Evaluate component ownership, evidence, frontend conformance and safe
 *   deprecation rules before Framework contracts are declared release-ready.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_GOVERNANCE_RELEASE_GATE_H
#define UMICOM_APPLICATION_GOVERNANCE_RELEASE_GATE_H

#include "umicom/application/governance/query.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named component gate severity values accepted by this public contract.
 */
typedef enum UmiComponentGateSeverity {
  UMI_COMPONENT_GATE_INFORMATION = 1,
  UMI_COMPONENT_GATE_WARNING = 2,
  UMI_COMPONENT_GATE_BLOCKER = 3
} UmiComponentGateSeverity;

/**
 * Represent the component release policy data shared with callers of this public contract.
 */
typedef struct UmiComponentReleasePolicy {
  uint32_t status_mask;
  uint32_t required_evidence;
  uint32_t required_frontends;
  int require_framework_owner;
  int require_deprecation_replacement;
} UmiComponentReleasePolicy;

/**
 * Represent the component gate finding data shared with callers of this public contract.
 */
typedef struct UmiComponentGateFinding {
  UmiComponentGateSeverity severity;
  char rule_id[32];
  char component_id[128];
  char message[UMI_COMPONENT_GOVERNANCE_TEXT_CAPACITY];
  uint32_t missing_evidence;
  uint32_t missing_frontends;
} UmiComponentGateFinding;

/**
 * Represent the component release report data shared with callers of this public contract.
 */
typedef struct UmiComponentReleaseReport {
  UmiComponentGateFinding findings[UMI_COMPONENT_GOVERNANCE_MAX_FINDINGS];
  size_t finding_count;
  size_t information_count;
  size_t warning_count;
  size_t blocker_count;
  size_t evaluated_component_count;
  int truncated;
  int passed;
} UmiComponentReleaseReport;

/**
 * Initialise component release policy from caller-provided values so later operations
 * receive a known state.
 */
void umi_component_release_policy_init(UmiComponentReleasePolicy *policy);
/**
 * Provide the component release gate evaluate operation used by this module and its client
 * applications.
 */
UmiStatus umi_component_release_gate_evaluate(const UmiComponentInventory *inventory,
                                              const UmiComponentReleasePolicy *policy,
                                              UmiComponentReleaseReport *out_report);
/**
 * Provide the component gate severity text operation used by this module and its client
 * applications.
 */
const char *umi_component_gate_severity_text(UmiComponentGateSeverity severity);

#ifdef __cplusplus
}
#endif

#endif
