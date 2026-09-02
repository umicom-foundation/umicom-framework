/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/authorisation.h
 *
 * PURPOSE:
 *   Evaluate capability requests through direct policy rules and reusable role
 *   grants with deny-by-default semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_AUTHORISATION_H
#define UMICOM_SECURITY_AUTHORISATION_H

#include "umicom/base/status.h"
#include "umicom/security/policy.h"
#include "umicom/security/role.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_AUTHORISATION_REASON_CAPACITY 256U
/**
 * Represent the authorisation decision data shared with callers of this public contract.
 */
typedef struct UmiAuthorisationDecision {
    int allowed;
    char reason[UMI_AUTHORISATION_REASON_CAPACITY];
} UmiAuthorisationDecision;
/**
 * Represent the authorisation service data shared with callers of this public contract.
 */
typedef struct UmiAuthorisationService UmiAuthorisationService;
/**
 * Initialise authorisation service from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_authorisation_service_create(UmiPolicyEngine *policy,
                                           UmiRoleRegistry *roles,
                                           UmiAuthorisationService **out_service);
/**
 * Release or reset state held by authorisation service so the same storage can be reused
 * safely.
 */
void umi_authorisation_service_destroy(UmiAuthorisationService *service);
/**
 * Provide the authorisation check operation used by this module and its client
 * applications.
 */
UmiStatus umi_authorisation_check(const UmiAuthorisationService *service,
                                  const char *principal,
                                  const char *role_id,
                                  const char *capability,
                                  const char *resource,
                                  UmiAuthorisationDecision *out_decision);
#ifdef __cplusplus
}
#endif

#endif
