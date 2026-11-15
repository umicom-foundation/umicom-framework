/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/authorengine_service.c
 *
 * PURPOSE:
 *   Coordinate AuthorEngine configuration, provider/runtime discovery,
 *   governed context, persistent conversations and provider health.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This object is the integration authority, not an inference engine.  It joins
 * existing Framework contracts and leaves execution to provider/process adapters.
 */
#include "umicom/ai/authorengine_service.h"
#include "provider_registry_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiAiAuthorEngineService {
    UmiAiRuntime *runtime;
    UmiAiAuthorEngineConfig config;
    UmiAiRuntimeCatalogue *catalogue;
    UmiAiContextBroker *context;
    UmiAiConversationArchive *conversations;
    UmiAiPolicy provider_policy;
    UmiAiPrivacyPolicy privacy_policy;
    uint32_t context_limit;
    uint32_t reserved_output_tokens;
    char active_session_id[UMI_AI_ID_CAPACITY];
    uint64_t revision;
    /* One serialized refresh owns callback publication until it returns. */
    int health_refresh_active;
};

/* A service revision identifies accepted service-owned changes. Refuse an
 * exhausted counter before publishing state; zero belongs to an invalid owner.
 * Child services exposed directly retain their own independent revisions. */
static UmiStatus CheckServiceMutationRevision(const UmiAiAuthorEngineService *service)
{
    if (service->revision == 0U) return UMI_STATUS_INVALID_STATE;
    if (service->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}

/* Provide the copy text operation used by this module and its client applications. */
static int copy_text(char *destination, size_t capacity, const char *source)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) return 0;
    written = snprintf(destination, capacity, "%s", source);
    return written >= 0 && (size_t)written < capacity;
}

/*
 * Initialise ai authorengine service from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_authorengine_service_create(
    UmiAiRuntime *runtime,
    const UmiAiAuthorEngineConfig *config,
    UmiAiAuthorEngineService **out_service)
{
    UmiAiAuthorEngineService *service;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || config == NULL || out_service == NULL ||
        umi_ai_authorengine_validate(config) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_service = NULL;
    service = (UmiAiAuthorEngineService *)calloc(1U, sizeof(*service));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    service->runtime = runtime;
    service->config = *config;
    service->provider_policy = runtime->policy;
    service->privacy_policy = umi_ai_privacy_policy_default();
    service->context_limit = 32768U;
    service->reserved_output_tokens = 2048U;
    service->revision = 1U;
    status = umi_ai_runtime_catalogue_create(&service->catalogue);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_context_broker_create(&service->context);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_conversation_archive_create(&service->conversations);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_ai_authorengine_service_destroy(service);
        return status;
    }
    *out_service = service;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ai authorengine service so the same storage can be reused
 * safely.
 */
/* Checked service destruction retains live health refresh owners; the legacy void API now delegates without discarding a refused service. The former implementation is retained for owner review. */
#if 0
void umi_ai_authorengine_service_destroy(UmiAiAuthorEngineService *service)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return;
    umi_ai_conversation_archive_destroy(service->conversations);
    umi_ai_context_broker_destroy(service->context);
    umi_ai_runtime_catalogue_destroy(service->catalogue);
    free(service);
}

#endif
UmiStatus UmiAiAuthorEngineServiceCheckIdle(const UmiAiAuthorEngineService *service)
{
    if (service == NULL) return UMI_STATUS_OK;
    return service->health_refresh_active ? UMI_STATUS_BUSY : UMI_STATUS_OK;
}

UmiStatus UmiAiAuthorEngineServiceDestroyChecked(UmiAiAuthorEngineService *service)
{
    UmiStatus status = UmiAiAuthorEngineServiceCheckIdle(service);
    if (status != UMI_STATUS_OK || service == NULL) return status;
    umi_ai_conversation_archive_destroy(service->conversations);
    umi_ai_context_broker_destroy(service->context);
    umi_ai_runtime_catalogue_destroy(service->catalogue);
    free(service);
    return UMI_STATUS_OK;
}

void umi_ai_authorengine_service_destroy(UmiAiAuthorEngineService *service)
{
    (void)UmiAiAuthorEngineServiceDestroyChecked(service);
}
/*
 * Provide the ai authorengine service catalogue operation used by this module and its
 * client applications.
 */
UmiAiRuntimeCatalogue *umi_ai_authorengine_service_catalogue(
    UmiAiAuthorEngineService *service)
{
    return service != NULL ? service->catalogue : NULL;
}

/*
 * Provide the ai authorengine service context operation used by this module and its client
 * applications.
 */
UmiAiContextBroker *umi_ai_authorengine_service_context(
    UmiAiAuthorEngineService *service)
{
    return service != NULL ? service->context : NULL;
}

/*
 * Provide the ai authorengine service conversations operation used by this module and its
 * client applications.
 */
UmiAiConversationArchive *umi_ai_authorengine_service_conversations(
    UmiAiAuthorEngineService *service)
{
    return service != NULL ? service->conversations : NULL;
}

/*
 * Provide the ai authorengine service set policy operation used by this module and its
 * client applications.
 */
/* Service mutations now check revision capacity before changing owned state so an exhausted counter cannot wrap. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_authorengine_service_set_policy(
    UmiAiAuthorEngineService *service,
    const UmiAiPolicy *provider_policy,
    const UmiAiPrivacyPolicy *privacy_policy,
    uint32_t context_limit,
    uint32_t reserved_output_tokens)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || provider_policy == NULL || privacy_policy == NULL ||
        context_limit == 0U || reserved_output_tokens >= context_limit) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    service->provider_policy = *provider_policy;
    service->privacy_policy = *privacy_policy;
    service->context_limit = context_limit;
    service->reserved_output_tokens = reserved_output_tokens;
    service->runtime->policy = *provider_policy;
    ++service->revision;
    return UMI_STATUS_OK;
}
#endif
/* Reject malformed privacy limits before publishing service or runtime policy. Revision capacity and all previous valid policy behavior remain enforced. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_authorengine_service_set_policy(
    UmiAiAuthorEngineService *service,
    const UmiAiPolicy *provider_policy,
    const UmiAiPrivacyPolicy *privacy_policy,
    uint32_t context_limit,
    uint32_t reserved_output_tokens)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || provider_policy == NULL || privacy_policy == NULL ||
        context_limit == 0U || reserved_output_tokens >= context_limit) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiStatus status = CheckServiceMutationRevision(service);
    if (status != UMI_STATUS_OK) return status;
    service->provider_policy = *provider_policy;
    service->privacy_policy = *privacy_policy;
    service->context_limit = context_limit;
    service->reserved_output_tokens = reserved_output_tokens;
    service->runtime->policy = *provider_policy;
    ++service->revision;
    return UMI_STATUS_OK;
}

#endif
UmiStatus umi_ai_authorengine_service_set_policy(
    UmiAiAuthorEngineService *service,
    const UmiAiPolicy *provider_policy,
    const UmiAiPrivacyPolicy *privacy_policy,
    uint32_t context_limit,
    uint32_t reserved_output_tokens)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || provider_policy == NULL || privacy_policy == NULL ||
        context_limit == 0U || reserved_output_tokens >= context_limit ||
        UmiAiPrivacyPolicyValidate(privacy_policy) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiStatus status = CheckServiceMutationRevision(service);
    if (status != UMI_STATUS_OK) return status;
    service->provider_policy = *provider_policy;
    service->privacy_policy = *privacy_policy;
    service->context_limit = context_limit;
    service->reserved_output_tokens = reserved_output_tokens;
    service->runtime->policy = *provider_policy;
    ++service->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the ai authorengine service register runtime operation used by this module and
 * its client applications.
 */
/* Service mutations now check revision capacity before changing owned state so an exhausted counter cannot wrap. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_authorengine_service_register_runtime(
    UmiAiAuthorEngineService *service,
    const UmiAiRuntimeDescriptor *descriptor)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ai_runtime_catalogue_upsert(service->catalogue, descriptor);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}
#endif
UmiStatus umi_ai_authorengine_service_register_runtime(
    UmiAiAuthorEngineService *service,
    const UmiAiRuntimeDescriptor *descriptor)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = CheckServiceMutationRevision(service);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_runtime_catalogue_upsert(service->catalogue, descriptor);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}

/*
 * Provide the provider model exists operation used by this module and its client
 * applications.
 */
static int provider_model_exists(UmiAiAuthorEngineService *service,
                                 const char *provider_id,
                                 const char *model_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_ai_runtime_catalogue_count(service->catalogue);
         ++index) {
        UmiAiRuntimeDescriptor descriptor;
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_ai_runtime_catalogue_at(
                service->catalogue, index, &descriptor) == UMI_STATUS_OK &&
            strcmp(descriptor.provider_id, provider_id) == 0 &&
            strcmp(descriptor.model_id, model_id) == 0) {
            return 1;
        }
    }
    return 0;
}

/*
 * Provide the ai authorengine service begin session operation used by this module and its
 * client applications.
 */
/* Service mutations now check revision capacity before changing owned state so an exhausted counter cannot wrap. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_authorengine_service_begin_session(
    UmiAiAuthorEngineService *service,
    const char *session_id,
    const char *provider_id,
    const char *model_id,
    const char *workspace_id,
    const char *title,
    uint64_t timestamp_ns)
{
    UmiAiConversationRecord record;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || session_id == NULL || provider_id == NULL ||
        model_id == NULL || workspace_id == NULL || title == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!provider_model_exists(service, provider_id, model_id)) {
        return UMI_STATUS_NOT_FOUND;
    }
    status = umi_ai_conversation_record_init(
        &record, session_id, provider_id, model_id, workspace_id, title,
        timestamp_ns);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_conversation_archive_upsert(
            service->conversations, &record);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && !copy_text(
            service->active_session_id, sizeof(service->active_session_id),
            session_id)) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}
#endif
UmiStatus umi_ai_authorengine_service_begin_session(
    UmiAiAuthorEngineService *service,
    const char *session_id,
    const char *provider_id,
    const char *model_id,
    const char *workspace_id,
    const char *title,
    uint64_t timestamp_ns)
{
    UmiAiConversationRecord record;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || session_id == NULL || provider_id == NULL ||
        model_id == NULL || workspace_id == NULL || title == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = CheckServiceMutationRevision(service);
    if (status != UMI_STATUS_OK) return status;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!provider_model_exists(service, provider_id, model_id)) {
        return UMI_STATUS_NOT_FOUND;
    }
    status = umi_ai_conversation_record_init(
        &record, session_id, provider_id, model_id, workspace_id, title,
        timestamp_ns);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_conversation_archive_upsert(
            service->conversations, &record);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && !copy_text(
            service->active_session_id, sizeof(service->active_session_id),
            session_id)) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}

/*
 * Provide the ai authorengine service add message operation used by this module and its
 * client applications.
 */
/* Service mutations now check revision capacity before changing owned state so an exhausted counter cannot wrap. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_authorengine_service_add_message(
    UmiAiAuthorEngineService *service,
    const char *session_id,
    const UmiAiMessage *message,
    uint64_t timestamp_ns)
{
    UmiAiConversationRecord record;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || session_id == NULL || message == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_ai_conversation_archive_find(
        service->conversations, session_id, &record);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_conversation_add(&record.conversation, message);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        record.updated_at_ns = timestamp_ns;
        ++record.revision;
        status = umi_ai_conversation_archive_upsert(
            service->conversations, &record);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}
#endif
UmiStatus umi_ai_authorengine_service_add_message(
    UmiAiAuthorEngineService *service,
    const char *session_id,
    const UmiAiMessage *message,
    uint64_t timestamp_ns)
{
    UmiAiConversationRecord record;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || session_id == NULL || message == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = CheckServiceMutationRevision(service);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_conversation_archive_find(
        service->conversations, session_id, &record);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        /* A loaded record can carry its final revision independently of the
         * service counter. Reject before modifying even the candidate message. */
        if (record.revision == 0U) return UMI_STATUS_INVALID_STATE;
        if (record.revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
        status = umi_ai_conversation_add(&record.conversation, message);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        record.updated_at_ns = timestamp_ns;
        ++record.revision;
        status = umi_ai_conversation_archive_upsert(
            service->conversations, &record);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}

/*
 * Provide the ai authorengine service plan context operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_authorengine_service_plan_context(
    UmiAiAuthorEngineService *service,
    const char *runtime_id,
    int sensitive_approved,
    UmiAiContextPlan *out_plan)
{
    UmiAiRuntimeDescriptor descriptor;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || runtime_id == NULL || out_plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_ai_runtime_catalogue_find(
        service->catalogue, runtime_id, &descriptor);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_policy_check_provider(
        &service->provider_policy, descriptor.provider_kind,
        sensitive_approved);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_ai_context_broker_plan(
        service->context,
        descriptor.context_tokens < service->context_limit
            ? descriptor.context_tokens : service->context_limit,
        descriptor.max_output_tokens < service->reserved_output_tokens
            ? descriptor.max_output_tokens : service->reserved_output_tokens,
        UMI_AI_CONTEXT_PLAN_SOURCE_MAX,
        descriptor.provider_kind,
        &service->privacy_policy,
        sensitive_approved,
        out_plan);
}

/* Read-only observations bind the three owners used by context planning.
 * They are deliberately separate from execution consent and live file state. */
UmiStatus UmiAiAuthorEnginePlanContextObserved(UmiAiAuthorEngineService *service,
    const char *runtime_id, int sensitive_approved, UmiAiContextPlan *out_plan,
    UmiAiContextPlanObservation *out_observation)
{
    UmiAiRuntimeDescriptor runtime;
    UmiSnapshotCapture context;
    UmiAiContextPlanObservation observation = {0};
    UmiStatus status;
    if (service == NULL || runtime_id == NULL || out_plan == NULL || out_observation == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (service->revision == 0U) return UMI_STATUS_INVALID_STATE;
    status = umi_ai_runtime_catalogue_find(service->catalogue, runtime_id, &runtime);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiContextBrokerCapture(service->context, &context);
    if (status != UMI_STATUS_OK) return status;
    observation.service_revision = service->revision;
    observation.context_revision = context.revision;
    observation.runtime_revision = runtime.revision;
    memcpy(observation.runtime_id, runtime.runtime_id, sizeof observation.runtime_id);
    /* Reuse the canonical planner's policy checks, budgets and complete-output
     * guarantee. With serialized ownership there is no callback or failing
     * operation between plan publication and observation publication. */
    status = umi_ai_authorengine_service_plan_context(service, observation.runtime_id,
        sensitive_approved, out_plan);
    if (status == UMI_STATUS_OK) *out_observation = observation;
    return status;
}

UmiStatus UmiAiAuthorEngineCheckContextObservation(const UmiAiAuthorEngineService *service,
    const UmiAiContextPlanObservation *observation)
{
    static const UmiSnapshotTextField fields[] = {
        {"runtime_id", offsetof(UmiAiContextPlanObservation, runtime_id),
            sizeof(((UmiAiContextPlanObservation *)0)->runtime_id), 1}
    };
    UmiAiRuntimeDescriptor runtime;
    UmiSnapshotCapture context;
    UmiStatus status;
    if (service == NULL || UmiSnapshotValidateTextFields(observation, sizeof *observation,
            fields, sizeof fields / sizeof fields[0], NULL) != UMI_STATUS_OK ||
        observation->service_revision == 0U || observation->context_revision == 0U ||
        observation->runtime_revision == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (service->revision == 0U) return UMI_STATUS_INVALID_STATE;
    if (service->revision != observation->service_revision) return UMI_STATUS_BUSY;
    status = UmiAiContextBrokerCapture(service->context, &context);
    if (status != UMI_STATUS_OK) return status;
    if (context.revision != observation->context_revision) return UMI_STATUS_BUSY;
    status = umi_ai_runtime_catalogue_find(service->catalogue, observation->runtime_id, &runtime);
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_BUSY;
    if (status != UMI_STATUS_OK) return status;
    return runtime.revision == observation->runtime_revision ? UMI_STATUS_OK : UMI_STATUS_BUSY;
}

/*
 * Provide the ai authorengine service refresh health operation used by this module and its
 * client applications.
 */
/* Health refresh now freezes the visit boundary and conditionally publishes against each captured runtime revision. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_authorengine_service_refresh_health(
    UmiAiAuthorEngineService *service,
    uint64_t timestamp_ns,
    size_t *out_healthy_runtimes)
{
    size_t index;
    size_t healthy = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || out_healthy_runtimes == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_ai_runtime_catalogue_count(service->catalogue);
         ++index) {
        UmiAiRuntimeDescriptor descriptor;
        UmiAiProvider *provider;
        UmiAiHealth health;
        UmiStatus status = umi_ai_runtime_catalogue_at(
            service->catalogue, index, &descriptor);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        (void)umi_ai_health_init(&health);
        (void)copy_text(health.provider_id, sizeof(health.provider_id),
                        descriptor.provider_id);
        provider = umi_ai_provider_registry_find(
            &service->runtime->providers, descriptor.provider_id);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (provider != NULL && provider->health != NULL) {
            status = provider->health(provider->instance, &health);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) {
                health.available = 0;
                (void)copy_text(health.message, sizeof(health.message),
                                "Provider health callback failed");
            }
        } else /* Apply this branch only when its contract condition is satisfied. */ if (descriptor.transport == UMI_AI_RUNTIME_PROCESS &&
                   descriptor.configured) {
            health.available = 0;
            (void)copy_text(
                health.message, sizeof(health.message),
                "AuthorEngine process is configured; a health probe is pending");
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            health.available = 0;
            (void)copy_text(health.message, sizeof(health.message),
                            "Provider runtime is not registered");
        }
        health.checked_at_ns = timestamp_ns;
        status = umi_ai_runtime_catalogue_set_health(
            service->catalogue, descriptor.runtime_id, &health);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Apply this branch only when its contract condition is satisfied. */
        if (health.available) ++healthy;
    }
    ++service->revision;
    *out_healthy_runtimes = healthy;
    return UMI_STATUS_OK;
}
#endif
/* Shared Framework refresh now supports descriptor-aware probes and prevents callback reentry while retaining provider-wide compatibility behavior. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_authorengine_service_refresh_health(
    UmiAiAuthorEngineService *service,
    uint64_t timestamp_ns,
    size_t *out_healthy_runtimes)
{
    size_t index;
    size_t healthy = 0U;
    size_t initial_count;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || out_healthy_runtimes == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (UmiAiRuntimeCatalogueValidate(service->catalogue) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Freeze the traversal boundary; callbacks may append future runtime records.
     * Publish only to the revision probed, never to a replacement configuration. */
    initial_count = umi_ai_runtime_catalogue_count(service->catalogue);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < initial_count;
         ++index) {
        UmiAiRuntimeDescriptor descriptor;
        UmiAiProvider *provider;
        UmiAiHealth health;
        UmiStatus status = umi_ai_runtime_catalogue_at(
            service->catalogue, index, &descriptor);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        (void)umi_ai_health_init(&health);
        (void)copy_text(health.provider_id, sizeof(health.provider_id),
                        descriptor.provider_id);
        provider = umi_ai_provider_registry_find(
            &service->runtime->providers, descriptor.provider_id);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (provider != NULL && provider->health != NULL) {
            status = provider->health(provider->instance, &health);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) {
                health.available = 0;
                (void)copy_text(health.message, sizeof(health.message),
                                "Provider health callback failed");
            }
        } else /* Apply this branch only when its contract condition is satisfied. */ if (descriptor.transport == UMI_AI_RUNTIME_PROCESS &&
                   descriptor.configured) {
            health.available = 0;
            (void)copy_text(
                health.message, sizeof(health.message),
                "AuthorEngine process is configured; a health probe is pending");
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            health.available = 0;
            (void)copy_text(health.message, sizeof(health.message),
                            "Provider runtime is not registered");
        }
        health.checked_at_ns = timestamp_ns;
        status = UmiAiRuntimeCatalogueSetHealthIfRevision(
            service->catalogue, descriptor.runtime_id, &health, descriptor.revision);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Apply this branch only when its contract condition is satisfied. */
        if (health.available) ++healthy;
    }
    ++service->revision;
    *out_healthy_runtimes = healthy;
    return UMI_STATUS_OK;
}
#endif
/* Service mutations now check revision capacity before changing owned state so an exhausted counter cannot wrap. The former implementation is retained for owner review. */
#if 0
static UmiStatus RefreshHealthRecords(
    UmiAiAuthorEngineService *service,
    uint64_t timestamp_ns,
    UmiAiRuntimeHealthProbeFn probe,
    void *context,
    size_t *out_healthy_runtimes)
{
    size_t index;
    size_t healthy = 0U;
    size_t initial_count;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || out_healthy_runtimes == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (UmiAiRuntimeCatalogueValidate(service->catalogue) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Freeze the traversal boundary; callbacks may append future runtime records.
     * Publish only to the revision probed, never to a replacement configuration. */
    initial_count = umi_ai_runtime_catalogue_count(service->catalogue);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < initial_count;
         ++index) {
        UmiAiRuntimeDescriptor descriptor;
        UmiAiProvider *provider;
        UmiAiHealth health;
        UmiStatus status = umi_ai_runtime_catalogue_at(
            service->catalogue, index, &descriptor);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        (void)umi_ai_health_init(&health);
        (void)copy_text(health.provider_id, sizeof(health.provider_id),
                        descriptor.provider_id);
        if (probe != NULL) {
            /* Endpoint-specific adapters receive the exact captured configuration.
             * Cancellation and transport errors are not unavailable observations. */
            status = probe(context, &descriptor, &health);
            if (status != UMI_STATUS_OK) return status;
        } else {
            provider = umi_ai_provider_registry_find(
                &service->runtime->providers, descriptor.provider_id);
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (provider != NULL && provider->health != NULL) {
                status = provider->health(provider->instance, &health);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status != UMI_STATUS_OK) {
                    health.available = 0;
                    (void)copy_text(health.message, sizeof(health.message),
                                    "Provider health callback failed");
                }
            } else /* Apply this branch only when its contract condition is satisfied. */ if (descriptor.transport == UMI_AI_RUNTIME_PROCESS &&
                       descriptor.configured) {
                health.available = 0;
                (void)copy_text(
                    health.message, sizeof(health.message),
                    "AuthorEngine process is configured; a health probe is pending");
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                health.available = 0;
                (void)copy_text(health.message, sizeof(health.message),
                                "Provider runtime is not registered");
            }
        }
        health.checked_at_ns = timestamp_ns;
        status = UmiAiRuntimeCatalogueSetHealthIfRevision(
            service->catalogue, descriptor.runtime_id, &health, descriptor.revision);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Apply this branch only when its contract condition is satisfied. */
        if (health.available) ++healthy;
    }
    ++service->revision;
    *out_healthy_runtimes = healthy;
    return UMI_STATUS_OK;
}
#endif
static UmiStatus RefreshHealthRecords(
    UmiAiAuthorEngineService *service,
    uint64_t timestamp_ns,
    UmiAiRuntimeHealthProbeFn probe,
    void *context,
    size_t *out_healthy_runtimes)
{
    size_t index;
    size_t healthy = 0U;
    size_t initial_count;
    UmiStatus admission;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || out_healthy_runtimes == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (UmiAiRuntimeCatalogueValidate(service->catalogue) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_STATE;
    }
    admission = CheckServiceMutationRevision(service);
    if (admission != UMI_STATUS_OK) return admission;
    /* Freeze the traversal boundary; callbacks may append future runtime records.
     * Publish only to the revision probed, never to a replacement configuration. */
    initial_count = umi_ai_runtime_catalogue_count(service->catalogue);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < initial_count;
         ++index) {
        /* Stop before another callback if a prior accepted write used the last
         * service revision. Callbacks may also consume a revision themselves. */
        admission = CheckServiceMutationRevision(service);
        if (admission != UMI_STATUS_OK) return admission;
        UmiAiRuntimeDescriptor descriptor;
        UmiAiProvider *provider;
        UmiAiHealth health;
        UmiStatus status = umi_ai_runtime_catalogue_at(
            service->catalogue, index, &descriptor);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        (void)umi_ai_health_init(&health);
        (void)copy_text(health.provider_id, sizeof(health.provider_id),
                        descriptor.provider_id);
        if (probe != NULL) {
            /* Endpoint-specific adapters receive the exact captured configuration.
             * Cancellation and transport errors are not unavailable observations. */
            status = probe(context, &descriptor, &health);
            if (status != UMI_STATUS_OK) return status;
        } else {
            provider = umi_ai_provider_registry_find(
                &service->runtime->providers, descriptor.provider_id);
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (provider != NULL && provider->health != NULL) {
                status = provider->health(provider->instance, &health);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status != UMI_STATUS_OK) {
                    health.available = 0;
                    (void)copy_text(health.message, sizeof(health.message),
                                    "Provider health callback failed");
                }
            } else /* Apply this branch only when its contract condition is satisfied. */ if (descriptor.transport == UMI_AI_RUNTIME_PROCESS &&
                       descriptor.configured) {
                health.available = 0;
                (void)copy_text(
                    health.message, sizeof(health.message),
                    "AuthorEngine process is configured; a health probe is pending");
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                health.available = 0;
                (void)copy_text(health.message, sizeof(health.message),
                                "Provider runtime is not registered");
            }
        }
        admission = CheckServiceMutationRevision(service);
        if (admission != UMI_STATUS_OK) return admission;
        health.checked_at_ns = timestamp_ns;
        status = UmiAiRuntimeCatalogueSetHealthIfRevision(
            service->catalogue, descriptor.runtime_id, &health, descriptor.revision);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Credit each accepted row before the next callback or failure. Views
         * can now notice partial publication through the service snapshot. */
        ++service->revision;
        /* Apply this branch only when its contract condition is satisfied. */
        if (health.available) ++healthy;
    }
    /* Preserve the established successful empty-refresh revision update. */
    if (initial_count == 0U) ++service->revision;
    *out_healthy_runtimes = healthy;
    return UMI_STATUS_OK;
}

/* Both entry points use the same owner guard, so a callback cannot recursively
 * start another refresh against partially updated records. Ordinary catalogue
 * edits remain possible and are checked by each captured record revision. */
/* Retain the borrowed provider runtime as well as the service across both legacy health callbacks and descriptor-aware probes. Existing record revision and error publication behavior stays in RefreshHealthRecords. The former implementation is retained for owner review. */
#if 0
static UmiStatus RefreshHealthOwned(
    UmiAiAuthorEngineService *service, uint64_t timestamp_ns,
    UmiAiRuntimeHealthProbeFn probe, void *context, size_t *out_healthy_runtimes)
{
    UmiStatus status;
    if (service == NULL || out_healthy_runtimes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (service->health_refresh_active) return UMI_STATUS_BUSY;
    service->health_refresh_active = 1;
    status = RefreshHealthRecords(service, timestamp_ns, probe, context, out_healthy_runtimes);
    service->health_refresh_active = 0;
    return status;
}

#endif
typedef struct AuthorEngineHealthCall {
    UmiAiAuthorEngineService *service;
    uint64_t timestamp_ns;
    UmiAiRuntimeHealthProbeFn probe;
    void *context;
    size_t *out_healthy_runtimes;
} AuthorEngineHealthCall;

static UmiStatus InvokeHealthRefresh(void *context)
{
    const AuthorEngineHealthCall *call = context;
    return RefreshHealthRecords(call->service, call->timestamp_ns, call->probe,
        call->context, call->out_healthy_runtimes);
}

static UmiStatus RefreshHealthOwned(
    UmiAiAuthorEngineService *service, uint64_t timestamp_ns,
    UmiAiRuntimeHealthProbeFn probe, void *context, size_t *out_healthy_runtimes)
{
    if (service == NULL || out_healthy_runtimes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiAuthorEngineServiceCheckIdle(service);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiRuntimeCheckIdle(service->runtime);
    if (status != UMI_STATUS_OK) return status;
    AuthorEngineHealthCall call = {service, timestamp_ns, probe, context, out_healthy_runtimes};
    int dispatched;
    /* Retain both the service and its borrowed runtime throughout traversal.
     * Catalogue edits still use captured revisions; registry mutation and
     * parent teardown wait until all callbacks and publication return. */
    service->health_refresh_active = 1;
    status = UmiAiProviderRegistryRunOwned(&service->runtime->providers,
        InvokeHealthRefresh, &call, &dispatched);
    service->health_refresh_active = 0;
    return status;
}
UmiStatus umi_ai_authorengine_service_refresh_health(
    UmiAiAuthorEngineService *service, uint64_t timestamp_ns,
    size_t *out_healthy_runtimes)
{
    return RefreshHealthOwned(service, timestamp_ns, NULL, NULL, out_healthy_runtimes);
}

UmiStatus UmiAiAuthorEngineRefreshRuntimeHealth(
    UmiAiAuthorEngineService *service, uint64_t timestamp_ns,
    UmiAiRuntimeHealthProbeFn probe, void *context, size_t *out_healthy_runtimes)
{
    if (probe == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return RefreshHealthOwned(service, timestamp_ns, probe, context, out_healthy_runtimes);
}


/*
 * Provide the ai authorengine service save session operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_authorengine_service_save_session(
    UmiAiAuthorEngineService *service,
    const char *session_id,
    const char *path)
{
    UmiAiConversationRecord record;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || session_id == NULL || path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_ai_conversation_archive_find(
        service->conversations, session_id, &record);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_ai_conversation_archive_save_record(
        &record, &service->privacy_policy, path);
}

/*
 * Provide the ai authorengine service load session operation used by this module and its
 * client applications.
 */
/* Service mutations now check revision capacity before changing owned state so an exhausted counter cannot wrap. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_authorengine_service_load_session(
    UmiAiAuthorEngineService *service,
    const char *path,
    UmiAiConversationRecord *out_record)
{
    UmiAiConversationRecord record;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || path == NULL || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_ai_conversation_archive_load_record(path, &record);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_conversation_archive_upsert(
            service->conversations, &record);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && !copy_text(
            service->active_session_id, sizeof(service->active_session_id),
            record.session.session_id)) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        *out_record = record;
        ++service->revision;
    }
    return status;
}
#endif
UmiStatus umi_ai_authorengine_service_load_session(
    UmiAiAuthorEngineService *service,
    const char *path,
    UmiAiConversationRecord *out_record)
{
    UmiAiConversationRecord record;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || path == NULL || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = CheckServiceMutationRevision(service);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_conversation_archive_load_record(path, &record);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_conversation_archive_upsert(
            service->conversations, &record);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && !copy_text(
            service->active_session_id, sizeof(service->active_session_id),
            record.session.session_id)) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        *out_record = record;
        ++service->revision;
    }
    return status;
}

/* Archive publication is the only fallible mutation. Prepare the report and
 * active-session text first, then publish service state after that write succeeds.
 * Shared archive admission owns stale/capacity checks; no application duplicates
 * those rules or interprets a file's old revision as the new stored identity. */
UmiStatus UmiAiAuthorEngineImportSession(UmiAiAuthorEngineService *service,
    const UmiAiConversationRecord *record, uint64_t expected_record_revision,
    UmiAiConversationImportReport *out_report)
{
    UmiAiConversationImportReport prepared = {0};
    UmiSnapshotCapture archive_before;
    UmiStatus status;
    if (service == NULL || out_report == NULL ||
        UmiAiConversationRecordValidate(record) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = CheckServiceMutationRevision(service);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiConversationArchiveCapture(service->conversations, &archive_before);
    if (status != UMI_STATUS_OK) return status;
    if (!copy_text(prepared.session_id, sizeof prepared.session_id, record->session.session_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    prepared.source_record_revision = record->revision;
    prepared.replaced = expected_record_revision != 0U;
    status = UmiAiConversationArchiveUpsertIfRevision(service->conversations,
        record, expected_record_revision);
    if (status != UMI_STATUS_OK) return status;
    /* Success proves revision capacity was available. All access is serialized
     * and there are no callbacks between capture and the accepted archive write. */
    prepared.stored_record_revision = archive_before.revision + 1U;
    memcpy(service->active_session_id, prepared.session_id, sizeof service->active_session_id);
    ++service->revision;
    prepared.service_revision = service->revision;
    *out_report = prepared;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiAuthorEngineLoadSessionBounded(UmiAiAuthorEngineService *service,
    const char *absolute_path, size_t maximum_bytes, uint64_t expected_record_revision,
    UmiAiConversationImportReport *out_report)
{
    UmiAiConversationRecord *candidate;
    UmiStatus status;
    if (service == NULL || absolute_path == NULL || absolute_path[0] == '\0' ||
        out_report == NULL || maximum_bytes == 0U ||
        maximum_bytes > UMI_AI_CONVERSATION_FILE_MAX_BYTES) return UMI_STATUS_INVALID_ARGUMENT;
    status = CheckServiceMutationRevision(service);
    if (status != UMI_STATUS_OK) return status;
    candidate = malloc(sizeof *candidate);
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiAiConversationArchiveLoadBounded(absolute_path, maximum_bytes, candidate);
    if (status == UMI_STATUS_OK)
        status = UmiAiAuthorEngineImportSession(service, candidate, expected_record_revision, out_report);
    free(candidate);
    return status;
}

/*
 * Provide the ai authorengine service snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_authorengine_service_snapshot(
    const UmiAiAuthorEngineService *service,
    UmiAiAuthorEngineServiceSnapshot *out_snapshot)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->revision = service->revision;
    out_snapshot->providers = umi_ai_runtime_catalogue_provider_count(
        service->catalogue);
    out_snapshot->runtimes = umi_ai_runtime_catalogue_count(
        service->catalogue);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < out_snapshot->runtimes; ++index) {
        UmiAiRuntimeDescriptor descriptor;
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_ai_runtime_catalogue_at(
                service->catalogue, index, &descriptor) == UMI_STATUS_OK &&
            descriptor.health.available) {
            ++out_snapshot->healthy_runtimes;
        }
    }
    out_snapshot->context_sources = umi_ai_context_broker_count(
        service->context);
    out_snapshot->sessions = umi_ai_conversation_archive_count(
        service->conversations);
    out_snapshot->context_limit = service->context_limit;
    out_snapshot->reserved_output_tokens = service->reserved_output_tokens;
    out_snapshot->remote_allowed = service->provider_policy.allow_remote;
    out_snapshot->persistence_allowed =
        service->privacy_policy.persist_sessions;
    (void)copy_text(out_snapshot->active_session_id,
                    sizeof(out_snapshot->active_session_id),
                    service->active_session_id);
    (void)copy_text(out_snapshot->executable,
                    sizeof(out_snapshot->executable),
                    service->config.executable);
    (void)copy_text(out_snapshot->workspace,
                    sizeof(out_snapshot->workspace),
                    service->config.workspace);
    return UMI_STATUS_OK;
}

/*
 * Provide the ai authorengine service privacy policy operation used by this module and its
 * client applications.
 */
const UmiAiPrivacyPolicy *umi_ai_authorengine_service_privacy_policy(
    const UmiAiAuthorEngineService *service)
{
    return service != NULL ? &service->privacy_policy : NULL;
}
