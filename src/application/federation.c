/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/federation.c
 *
 * PURPOSE:
 *   Implement capability-routed, policy-mediated application federation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/federation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/integration/registry.h"
#include "umicom/runtime/capability_catalogue.h"

#define UMI_FEDERATION_MAX_APPLICATIONS UMI_INTEGRATION_MAX_APPLICATIONS
#define UMI_FEDERATION_MAX_PROVIDERS 256U

typedef struct UmiFederationProvider {
    const UmiApplicationDefinition *application;
    const char *capability_id;
    UmiFederationHandler handler;
    void *handler_context;
} UmiFederationProvider;

struct UmiFederationRouter {
    /*
     * The established Integration Fabric remains the authoritative discovery
     * and transport-facing registry. Federation adds capability selection and
     * policy above it; it does not create a competing application bus.
     */
    UmiIntegrationRegistry integration_registry;
    const UmiApplicationDefinition *applications[UMI_FEDERATION_MAX_APPLICATIONS];
    UmiFederationProvider providers[UMI_FEDERATION_MAX_PROVIDERS];
    size_t application_count;
    size_t provider_count;
    UmiFederationPolicy policy;
    void *policy_context;
    uint64_t dispatch_count;
    uint64_t denied_count;
    uint64_t failed_count;
};

/*
 * Provide the register with integration fabric operation used by this module and its
 * client applications.
 */
static UmiStatus register_with_integration_fabric(
    UmiFederationRouter *router,
    const UmiApplicationDefinition *definition)
{
    UmiIntegrationApplication application;
    size_t index;
    UmiStatus status;

    umi_integration_application_init(&application);
    status = umi_integration_application_set_identity(
        &application, definition->application_id, definition->display_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    application.enabled = true;
    application.frontends = definition->frontends;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < definition->required_capability_count; ++index) {
        status = umi_integration_application_add_capability(
            &application, definition->required_capabilities[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < definition->optional_capability_count; ++index) {
        status = umi_integration_application_add_capability(
            &application, definition->optional_capabilities[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK && status != UMI_STATUS_ALREADY_EXISTS)
            return status;
    }
    return umi_integration_registry_register(
        &router->integration_registry, &application);
}

/* Provide the find application operation used by this module and its client applications. */
static const UmiApplicationDefinition *find_application(
    const UmiFederationRouter *router, const char *application_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (router == NULL || application_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < router->application_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(router->applications[index]->application_id,
                   application_id) == 0) return router->applications[index];
    }
    return NULL;
}

/* Provide the valid request operation used by this module and its client applications. */
static int valid_request(const UmiFederationRequest *request)
{
    return request != NULL && request->structure_size >= sizeof(*request) &&
        request->message_id != NULL && request->message_id[0] != '\0' &&
        request->source_application_id != NULL &&
        request->source_application_id[0] != '\0' &&
        request->capability_id != NULL && request->capability_id[0] != '\0' &&
        request->operation_id != NULL && request->operation_id[0] != '\0' &&
        request->kind >= UMI_FEDERATION_COMMAND &&
        request->kind <= UMI_FEDERATION_EVENT &&
        (request->payload_size == 0U || request->payload != NULL);
}

/*
 * Initialise federation router from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_federation_router_create(UmiFederationRouter **out_router)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_router == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_router = (UmiFederationRouter *)calloc(1U, sizeof(**out_router));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (*out_router != NULL)
        umi_integration_registry_init(&(*out_router)->integration_registry);
    return *out_router != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}

/*
 * Release or reset state held by federation router so the same storage can be reused
 * safely.
 */
void umi_federation_router_destroy(UmiFederationRouter *router)
{
    free(router);
}

/*
 * Provide the federation router register application operation used by this module and its
 * client applications.
 */
UmiStatus umi_federation_router_register_application(
    UmiFederationRouter *router,
    const UmiApplicationDefinition *definition)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (router == NULL ||
        umi_application_definition_validate(definition) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (find_application(router, definition->application_id) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (router->application_count >= UMI_FEDERATION_MAX_APPLICATIONS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = register_with_integration_fabric(router, definition);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    router->applications[router->application_count++] = definition;
    return UMI_STATUS_OK;
}

/*
 * Provide the federation router find application operation used by this module and its
 * client applications.
 */
const UmiApplicationDefinition *umi_federation_router_find_application(
    const UmiFederationRouter *router,
    const char *application_id)
{
    return find_application(router, application_id);
}

/*
 * Provide the federation router register provider operation used by this module and its
 * client applications.
 */
UmiStatus umi_federation_router_register_provider(
    UmiFederationRouter *router,
    const char *application_id,
    const char *capability_id,
    UmiFederationHandler handler,
    void *handler_context)
{
    const UmiApplicationDefinition *application;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (router == NULL || application_id == NULL || capability_id == NULL ||
        handler == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    application = find_application(router, application_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (application == NULL) return UMI_STATUS_NOT_FOUND;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_framework_capability_catalogue_find(capability_id) == NULL ||
        !umi_application_definition_declares_capability(application,
                                                        capability_id))
        return UMI_STATUS_PERMISSION_DENIED;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < router->provider_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (router->providers[index].application == application &&
            strcmp(router->providers[index].capability_id,
                   capability_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (router->provider_count >= UMI_FEDERATION_MAX_PROVIDERS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    router->providers[router->provider_count++] = (UmiFederationProvider){
        application, capability_id, handler, handler_context};
    return UMI_STATUS_OK;
}

/*
 * Provide the federation router set policy operation used by this module and its client
 * applications.
 */
void umi_federation_router_set_policy(
    UmiFederationRouter *router,
    UmiFederationPolicy policy,
    void *policy_context)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (router == NULL) return;
    router->policy = policy;
    router->policy_context = policy_context;
}

/*
 * Perform federation router through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_federation_router_dispatch(
    UmiFederationRouter *router,
    const UmiFederationRequest *request,
    UmiFederationResponse *response)
{
    const UmiApplicationDefinition *source;
    size_t index;
    size_t delivered = 0U;
    UmiStatus first_failure = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (router == NULL || !valid_request(request))
        return UMI_STATUS_INVALID_ARGUMENT;
    source = find_application(router, request->source_application_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_application_definition_declares_capability(
            source, request->capability_id)) {
        router->denied_count += 1U;
        return UMI_STATUS_PERMISSION_DENIED;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < router->provider_count; ++index) {
        UmiFederationProvider *provider = &router->providers[index];
        UmiStatus status;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(provider->capability_id, request->capability_id) != 0)
            continue;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (router->policy != NULL && !router->policy(
                router->policy_context, request,
                provider->application->application_id)) {
            router->denied_count += 1U;
            continue;
        }
        status = provider->handler(provider->handler_context, request,
                                   response);
        router->dispatch_count += 1U;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            delivered += 1U;
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (response != NULL) {
                (void)snprintf(response->provider_application_id,
                               sizeof(response->provider_application_id),
                               "%s", provider->application->application_id);
            }
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            router->failed_count += 1U;
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (first_failure == UMI_STATUS_OK) first_failure = status;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (request->kind != UMI_FEDERATION_EVENT) break;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (delivered > 0U) return UMI_STATUS_OK;
    return first_failure != UMI_STATUS_OK ? first_failure
                                          : UMI_STATUS_UNAVAILABLE;
}

/*
 * Provide the federation router stats operation used by this module and its client
 * applications.
 */
UmiFederationStats umi_federation_router_stats(
    const UmiFederationRouter *router)
{
    UmiFederationStats stats = {0};
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (router == NULL) return stats;
    stats.application_count = router->application_count;
    stats.provider_count = router->provider_count;
    stats.dispatch_count = router->dispatch_count;
    stats.denied_count = router->denied_count;
    stats.failed_count = router->failed_count;
    return stats;
}
