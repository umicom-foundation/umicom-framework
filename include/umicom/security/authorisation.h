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
typedef struct UmiAuthorisationDecision {
    int allowed;
    char reason[UMI_AUTHORISATION_REASON_CAPACITY];
} UmiAuthorisationDecision;
typedef struct UmiAuthorisationService UmiAuthorisationService;
UmiStatus umi_authorisation_service_create(UmiPolicyEngine *policy,
                                           UmiRoleRegistry *roles,
                                           UmiAuthorisationService **out_service);
void umi_authorisation_service_destroy(UmiAuthorisationService *service);
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
