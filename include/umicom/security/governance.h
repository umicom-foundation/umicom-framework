/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/governance.h
 *
 * PURPOSE:
 *   Publish the public governance contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_GOVERNANCE_H
#define UMICOM_SECURITY_GOVERNANCE_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#define UMI_SECURITY_GOVERNANCE_ID_CAPACITY 128U
#define UMI_SECURITY_GOVERNANCE_NAME_CAPACITY 192U
#define UMI_SECURITY_GOVERNANCE_TEXT_CAPACITY 384U
#define UMI_SECURITY_GOVERNANCE_PATH_CAPACITY 512U
#define UMI_SECURITY_GOVERNANCE_MAX_IDENTITIES 128U
#define UMI_SECURITY_GOVERNANCE_MAX_ROLES 64U
#define UMI_SECURITY_GOVERNANCE_MAX_GRANTS 64U
#define UMI_SECURITY_GOVERNANCE_MAX_ASSIGNMENTS 256U
#define UMI_SECURITY_GOVERNANCE_MAX_PERMISSIONS 256U
#define UMI_SECURITY_GOVERNANCE_MAX_SECRETS 128U
#define UMI_SECURITY_GOVERNANCE_MAX_APPROVALS 128U
#define UMI_SECURITY_GOVERNANCE_MAX_SIGNERS 64U
#define UMI_SECURITY_GOVERNANCE_MAX_AUDIT 512U
/**
 * List the named security risk values accepted by this public contract.
 */
typedef enum UmiSecurityRisk { UMI_SECURITY_RISK_LOW = 1, UMI_SECURITY_RISK_MEDIUM, UMI_SECURITY_RISK_HIGH, UMI_SECURITY_RISK_CRITICAL } UmiSecurityRisk;
/**
 * List the named security environment values accepted by this public contract.
 */
typedef enum UmiSecurityEnvironment { UMI_SECURITY_ENV_DEVELOPMENT = 1, UMI_SECURITY_ENV_TEST, UMI_SECURITY_ENV_PRODUCTION } UmiSecurityEnvironment;
/**
 * List the named security approval subject values accepted by this public contract.
 */
typedef enum UmiSecurityApprovalSubject { UMI_SECURITY_APPROVAL_PLUGIN = 1, UMI_SECURITY_APPROVAL_TOOL, UMI_SECURITY_APPROVAL_AI, UMI_SECURITY_APPROVAL_PACKAGE } UmiSecurityApprovalSubject;
/**
 * List the named security approval state values accepted by this public contract.
 */
typedef enum UmiSecurityApprovalState { UMI_SECURITY_APPROVAL_PENDING = 1, UMI_SECURITY_APPROVAL_APPROVED, UMI_SECURITY_APPROVAL_DENIED, UMI_SECURITY_APPROVAL_EXPIRED } UmiSecurityApprovalState;
/**
 * List the named security audit outcome values accepted by this public contract.
 */
typedef enum UmiSecurityAuditOutcome { UMI_SECURITY_AUDIT_ALLOWED = 1, UMI_SECURITY_AUDIT_DENIED, UMI_SECURITY_AUDIT_ERROR } UmiSecurityAuditOutcome;
/**
 * Represent the security governance decision data shared with callers of this public
 * contract.
 */
typedef struct UmiSecurityGovernanceDecision { bool allowed; bool approval_required; char reason[UMI_SECURITY_GOVERNANCE_TEXT_CAPACITY]; } UmiSecurityGovernanceDecision;
#endif
