/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/context.h
 *
 * PURPOSE:
 *   Own the reusable security registries and policy services required by a
 *   complete Umicom application process.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_CONTEXT_H
#define UMICOM_SECURITY_CONTEXT_H

#include "umicom/base/status.h"
#include "umicom/security/identity.h"
#include "umicom/security/role.h"
#include "umicom/security/session.h"
#include "umicom/security/credential.h"
#include "umicom/security/secret_provider.h"
#include "umicom/security/redaction.h"
#include "umicom/security/workspace_trust.h"
#include "umicom/security/event.h"
#include "umicom/security/authorisation.h"
#include "umicom/security/policy.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSecurityContext UmiSecurityContext;
UmiStatus umi_security_context_create(UmiSecurityContext **out_context);
void umi_security_context_destroy(UmiSecurityContext *context);
UmiIdentityRegistry *umi_security_context_identities(UmiSecurityContext *context);
UmiRoleRegistry *umi_security_context_roles(UmiSecurityContext *context);
UmiSessionRegistry *umi_security_context_sessions(UmiSecurityContext *context);
UmiCredentialRegistry *umi_security_context_credentials(UmiSecurityContext *context);
UmiSecretProviderRegistry *umi_security_context_secret_providers(UmiSecurityContext *context);
UmiRedactor *umi_security_context_redactor(UmiSecurityContext *context);
UmiWorkspaceTrustStore *umi_security_context_workspace_trust(UmiSecurityContext *context);
UmiSecurityEventLog *umi_security_context_events(UmiSecurityContext *context);
UmiPolicyEngine *umi_security_context_policy(UmiSecurityContext *context);
UmiAuthorisationService *umi_security_context_authorisation(UmiSecurityContext *context);
#ifdef __cplusplus
}
#endif

#endif
