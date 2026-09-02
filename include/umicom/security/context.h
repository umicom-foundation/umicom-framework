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
/**
 * Represent the security context data shared with callers of this public contract.
 */
typedef struct UmiSecurityContext UmiSecurityContext;
/**
 * Initialise security context from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_security_context_create(UmiSecurityContext **out_context);
/**
 * Release or reset state held by security context so the same storage can be reused
 * safely.
 */
void umi_security_context_destroy(UmiSecurityContext *context);
/**
 * Provide the security context identities operation used by this module and its client
 * applications.
 */
UmiIdentityRegistry *umi_security_context_identities(UmiSecurityContext *context);
/**
 * Provide the security context roles operation used by this module and its client
 * applications.
 */
UmiRoleRegistry *umi_security_context_roles(UmiSecurityContext *context);
/**
 * Provide the security context sessions operation used by this module and its client
 * applications.
 */
UmiSessionRegistry *umi_security_context_sessions(UmiSecurityContext *context);
/**
 * Provide the security context credentials operation used by this module and its client
 * applications.
 */
UmiCredentialRegistry *umi_security_context_credentials(UmiSecurityContext *context);
/**
 * Provide the security context secret providers operation used by this module and its
 * client applications.
 */
UmiSecretProviderRegistry *umi_security_context_secret_providers(UmiSecurityContext *context);
/**
 * Provide the security context redactor operation used by this module and its client
 * applications.
 */
UmiRedactor *umi_security_context_redactor(UmiSecurityContext *context);
/**
 * Provide the security context workspace trust operation used by this module and its
 * client applications.
 */
UmiWorkspaceTrustStore *umi_security_context_workspace_trust(UmiSecurityContext *context);
/**
 * Provide the security context events operation used by this module and its client
 * applications.
 */
UmiSecurityEventLog *umi_security_context_events(UmiSecurityContext *context);
/**
 * Provide the security context policy operation used by this module and its client
 * applications.
 */
UmiPolicyEngine *umi_security_context_policy(UmiSecurityContext *context);
/**
 * Provide the security context authorisation operation used by this module and its client
 * applications.
 */
UmiAuthorisationService *umi_security_context_authorisation(UmiSecurityContext *context);
#ifdef __cplusplus
}
#endif

#endif
