/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/authorisation.c
 *
 * PURPOSE:
 *   Combine explicit policy rules and role permission grants into one
 *   inspectable decision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/security/authorisation.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
struct UmiAuthorisationService { UmiPolicyEngine *policy; UmiRoleRegistry *roles; };
UmiStatus umi_authorisation_service_create(UmiPolicyEngine *policy, UmiRoleRegistry *roles, UmiAuthorisationService **out_service)
{
    UmiAuthorisationService *service;
    if (policy == NULL || roles == NULL || out_service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_service = NULL; service = (UmiAuthorisationService *)calloc(1U, sizeof(*service)); if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    service->policy = policy; service->roles = roles; *out_service = service; return UMI_STATUS_OK;
}
void umi_authorisation_service_destroy(UmiAuthorisationService *service) { free(service); }
UmiStatus umi_authorisation_check(const UmiAuthorisationService *service, const char *principal, const char *role_id, const char *capability, const char *resource, UmiAuthorisationDecision *out_decision)
{
    UmiPolicyDecision policy;
    if (service == NULL || principal == NULL || capability == NULL || resource == NULL || out_decision == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    out_decision->allowed = 0; out_decision->reason[0] = '\0';
    policy = umi_policy_engine_authorize(service->policy, principal, capability, resource);
    if (policy.effect == UMI_POLICY_ALLOW) {
        out_decision->allowed = 1; (void)snprintf(out_decision->reason, sizeof(out_decision->reason), "allowed by explicit policy"); return UMI_STATUS_OK;
    }
    if (role_id != NULL && role_id[0] != '\0' && umi_role_registry_allows(service->roles, role_id, capability)) {
        out_decision->allowed = 1; (void)snprintf(out_decision->reason, sizeof(out_decision->reason), "allowed by role %s", role_id); return UMI_STATUS_OK;
    }
    (void)snprintf(out_decision->reason, sizeof(out_decision->reason), "denied: no matching policy or role grant"); return UMI_STATUS_OK;
}
