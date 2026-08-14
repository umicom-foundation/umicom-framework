/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/context.c
 *
 * PURPOSE:
 *   Construct and destroy the complete process-level security context in
 *   dependency order.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/security/context.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
struct UmiSecurityContext {
    UmiIdentityRegistry *identities; UmiRoleRegistry *roles; UmiSessionRegistry *sessions;
    UmiCredentialRegistry *credentials; UmiSecretProviderRegistry *secret_providers;
    UmiRedactor *redactor; UmiWorkspaceTrustStore *workspace_trust;
    UmiSecurityEventLog *events; UmiPolicyEngine *policy; UmiAuthorisationService *authorisation;
};
static void context_destroy_partial(UmiSecurityContext *context)
{
    if (context == NULL) return;
    umi_authorisation_service_destroy(context->authorisation);
    umi_policy_engine_destroy(context->policy);
    umi_security_event_log_destroy(context->events);
    umi_workspace_trust_store_destroy(context->workspace_trust);
    umi_redactor_destroy(context->redactor);
    umi_secret_provider_registry_destroy(context->secret_providers);
    umi_credential_registry_destroy(context->credentials);
    umi_session_registry_destroy(context->sessions);
    umi_role_registry_destroy(context->roles);
    umi_identity_registry_destroy(context->identities);
    free(context);
}
UmiStatus umi_security_context_create(UmiSecurityContext **out_context)
{
    UmiSecurityContext *context; UmiSecretProvider environment; UmiStatus status;
    if (out_context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_context = NULL; context = (UmiSecurityContext *)calloc(1U, sizeof(*context)); if (context == NULL) return UMI_STATUS_OUT_OF_MEMORY;
#define CREATE(call) do { status = (call); if (status != UMI_STATUS_OK) { context_destroy_partial(context); return status; } } while (0)
    CREATE(umi_identity_registry_create(&context->identities));
    CREATE(umi_role_registry_create(&context->roles));
    CREATE(umi_session_registry_create(&context->sessions));
    CREATE(umi_credential_registry_create(&context->credentials));
    CREATE(umi_secret_provider_registry_create(&context->secret_providers));
    CREATE(umi_redactor_create(&context->redactor));
    CREATE(umi_workspace_trust_store_create(&context->workspace_trust));
    CREATE(umi_security_event_log_create(&context->events));
    CREATE(umi_policy_engine_create(&context->policy));
    CREATE(umi_authorisation_service_create(context->policy, context->roles, &context->authorisation));
    CREATE(umi_secret_provider_environment(&environment));
    CREATE(umi_secret_provider_registry_add(context->secret_providers, "env", &environment));
#undef CREATE
    *out_context = context; return UMI_STATUS_OK;
}
void umi_security_context_destroy(UmiSecurityContext *context) { context_destroy_partial(context); }
#define GETTER(name, type, field) type *name(UmiSecurityContext *context) { return context != NULL ? context->field : NULL; }
GETTER(umi_security_context_identities, UmiIdentityRegistry, identities)
GETTER(umi_security_context_roles, UmiRoleRegistry, roles)
GETTER(umi_security_context_sessions, UmiSessionRegistry, sessions)
GETTER(umi_security_context_credentials, UmiCredentialRegistry, credentials)
GETTER(umi_security_context_secret_providers, UmiSecretProviderRegistry, secret_providers)
GETTER(umi_security_context_redactor, UmiRedactor, redactor)
GETTER(umi_security_context_workspace_trust, UmiWorkspaceTrustStore, workspace_trust)
GETTER(umi_security_context_events, UmiSecurityEventLog, events)
GETTER(umi_security_context_policy, UmiPolicyEngine, policy)
GETTER(umi_security_context_authorisation, UmiAuthorisationService, authorisation)
#undef GETTER
