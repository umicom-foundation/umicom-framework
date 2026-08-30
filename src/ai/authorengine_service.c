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

/* BEGINNER NOTE:
 * This object is the integration authority, not an inference engine.  It joins
 * existing Framework contracts and leaves execution to provider/process adapters.
 */
#include "umicom/ai/authorengine_service.h"

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
};

static int copy_text(char *destination, size_t capacity, const char *source)
{
    int written;
    if (destination == NULL || capacity == 0U || source == NULL) return 0;
    written = snprintf(destination, capacity, "%s", source);
    return written >= 0 && (size_t)written < capacity;
}

UmiStatus umi_ai_authorengine_service_create(
    UmiAiRuntime *runtime,
    const UmiAiAuthorEngineConfig *config,
    UmiAiAuthorEngineService **out_service)
{
    UmiAiAuthorEngineService *service;
    UmiStatus status;
    if (runtime == NULL || config == NULL || out_service == NULL ||
        umi_ai_authorengine_validate(config) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_service = NULL;
    service = (UmiAiAuthorEngineService *)calloc(1U, sizeof(*service));
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    service->runtime = runtime;
    service->config = *config;
    service->provider_policy = runtime->policy;
    service->privacy_policy = umi_ai_privacy_policy_default();
    service->context_limit = 32768U;
    service->reserved_output_tokens = 2048U;
    service->revision = 1U;
    status = umi_ai_runtime_catalogue_create(&service->catalogue);
    if (status == UMI_STATUS_OK) {
        status = umi_ai_context_broker_create(&service->context);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ai_conversation_archive_create(&service->conversations);
    }
    if (status != UMI_STATUS_OK) {
        umi_ai_authorengine_service_destroy(service);
        return status;
    }
    *out_service = service;
    return UMI_STATUS_OK;
}

void umi_ai_authorengine_service_destroy(UmiAiAuthorEngineService *service)
{
    if (service == NULL) return;
    umi_ai_conversation_archive_destroy(service->conversations);
    umi_ai_context_broker_destroy(service->context);
    umi_ai_runtime_catalogue_destroy(service->catalogue);
    free(service);
}

UmiAiRuntimeCatalogue *umi_ai_authorengine_service_catalogue(
    UmiAiAuthorEngineService *service)
{
    return service != NULL ? service->catalogue : NULL;
}

UmiAiContextBroker *umi_ai_authorengine_service_context(
    UmiAiAuthorEngineService *service)
{
    return service != NULL ? service->context : NULL;
}

UmiAiConversationArchive *umi_ai_authorengine_service_conversations(
    UmiAiAuthorEngineService *service)
{
    return service != NULL ? service->conversations : NULL;
}

UmiStatus umi_ai_authorengine_service_set_policy(
    UmiAiAuthorEngineService *service,
    const UmiAiPolicy *provider_policy,
    const UmiAiPrivacyPolicy *privacy_policy,
    uint32_t context_limit,
    uint32_t reserved_output_tokens)
{
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

UmiStatus umi_ai_authorengine_service_register_runtime(
    UmiAiAuthorEngineService *service,
    const UmiAiRuntimeDescriptor *descriptor)
{
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ai_runtime_catalogue_upsert(service->catalogue, descriptor);
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}

static int provider_model_exists(UmiAiAuthorEngineService *service,
                                 const char *provider_id,
                                 const char *model_id)
{
    size_t index;
    for (index = 0U;
         index < umi_ai_runtime_catalogue_count(service->catalogue);
         ++index) {
        UmiAiRuntimeDescriptor descriptor;
        if (umi_ai_runtime_catalogue_at(
                service->catalogue, index, &descriptor) == UMI_STATUS_OK &&
            strcmp(descriptor.provider_id, provider_id) == 0 &&
            strcmp(descriptor.model_id, model_id) == 0) {
            return 1;
        }
    }
    return 0;
}

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
    if (service == NULL || session_id == NULL || provider_id == NULL ||
        model_id == NULL || workspace_id == NULL || title == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!provider_model_exists(service, provider_id, model_id)) {
        return UMI_STATUS_NOT_FOUND;
    }
    status = umi_ai_conversation_record_init(
        &record, session_id, provider_id, model_id, workspace_id, title,
        timestamp_ns);
    if (status == UMI_STATUS_OK) {
        status = umi_ai_conversation_archive_upsert(
            service->conversations, &record);
    }
    if (status == UMI_STATUS_OK && !copy_text(
            service->active_session_id, sizeof(service->active_session_id),
            session_id)) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}

UmiStatus umi_ai_authorengine_service_add_message(
    UmiAiAuthorEngineService *service,
    const char *session_id,
    const UmiAiMessage *message,
    uint64_t timestamp_ns)
{
    UmiAiConversationRecord record;
    UmiStatus status;
    if (service == NULL || session_id == NULL || message == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_ai_conversation_archive_find(
        service->conversations, session_id, &record);
    if (status == UMI_STATUS_OK) {
        status = umi_ai_conversation_add(&record.conversation, message);
    }
    if (status == UMI_STATUS_OK) {
        record.updated_at_ns = timestamp_ns;
        ++record.revision;
        status = umi_ai_conversation_archive_upsert(
            service->conversations, &record);
    }
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}

UmiStatus umi_ai_authorengine_service_plan_context(
    UmiAiAuthorEngineService *service,
    const char *runtime_id,
    int sensitive_approved,
    UmiAiContextPlan *out_plan)
{
    UmiAiRuntimeDescriptor descriptor;
    UmiStatus status;
    if (service == NULL || runtime_id == NULL || out_plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_ai_runtime_catalogue_find(
        service->catalogue, runtime_id, &descriptor);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_policy_check_provider(
        &service->provider_policy, descriptor.provider_kind,
        sensitive_approved);
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

UmiStatus umi_ai_authorengine_service_refresh_health(
    UmiAiAuthorEngineService *service,
    uint64_t timestamp_ns,
    size_t *out_healthy_runtimes)
{
    size_t index;
    size_t healthy = 0U;
    if (service == NULL || out_healthy_runtimes == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U;
         index < umi_ai_runtime_catalogue_count(service->catalogue);
         ++index) {
        UmiAiRuntimeDescriptor descriptor;
        UmiAiProvider *provider;
        UmiAiHealth health;
        UmiStatus status = umi_ai_runtime_catalogue_at(
            service->catalogue, index, &descriptor);
        if (status != UMI_STATUS_OK) return status;
        (void)umi_ai_health_init(&health);
        (void)copy_text(health.provider_id, sizeof(health.provider_id),
                        descriptor.provider_id);
        provider = umi_ai_provider_registry_find(
            &service->runtime->providers, descriptor.provider_id);
        if (provider != NULL && provider->health != NULL) {
            status = provider->health(provider->instance, &health);
            if (status != UMI_STATUS_OK) {
                health.available = 0;
                (void)copy_text(health.message, sizeof(health.message),
                                "Provider health callback failed");
            }
        } else if (descriptor.transport == UMI_AI_RUNTIME_PROCESS &&
                   descriptor.configured) {
            health.available = 0;
            (void)copy_text(
                health.message, sizeof(health.message),
                "AuthorEngine process is configured; a health probe is pending");
        } else {
            health.available = 0;
            (void)copy_text(health.message, sizeof(health.message),
                            "Provider runtime is not registered");
        }
        health.checked_at_ns = timestamp_ns;
        status = umi_ai_runtime_catalogue_set_health(
            service->catalogue, descriptor.runtime_id, &health);
        if (status != UMI_STATUS_OK) return status;
        if (health.available) ++healthy;
    }
    ++service->revision;
    *out_healthy_runtimes = healthy;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_authorengine_service_save_session(
    UmiAiAuthorEngineService *service,
    const char *session_id,
    const char *path)
{
    UmiAiConversationRecord record;
    UmiStatus status;
    if (service == NULL || session_id == NULL || path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_ai_conversation_archive_find(
        service->conversations, session_id, &record);
    if (status != UMI_STATUS_OK) return status;
    return umi_ai_conversation_archive_save_record(
        &record, &service->privacy_policy, path);
}

UmiStatus umi_ai_authorengine_service_load_session(
    UmiAiAuthorEngineService *service,
    const char *path,
    UmiAiConversationRecord *out_record)
{
    UmiAiConversationRecord record;
    UmiStatus status;
    if (service == NULL || path == NULL || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_ai_conversation_archive_load_record(path, &record);
    if (status == UMI_STATUS_OK) {
        status = umi_ai_conversation_archive_upsert(
            service->conversations, &record);
    }
    if (status == UMI_STATUS_OK && !copy_text(
            service->active_session_id, sizeof(service->active_session_id),
            record.session.session_id)) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (status == UMI_STATUS_OK) {
        *out_record = record;
        ++service->revision;
    }
    return status;
}

UmiStatus umi_ai_authorengine_service_snapshot(
    const UmiAiAuthorEngineService *service,
    UmiAiAuthorEngineServiceSnapshot *out_snapshot)
{
    size_t index;
    if (service == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->revision = service->revision;
    out_snapshot->providers = umi_ai_runtime_catalogue_provider_count(
        service->catalogue);
    out_snapshot->runtimes = umi_ai_runtime_catalogue_count(
        service->catalogue);
    for (index = 0U; index < out_snapshot->runtimes; ++index) {
        UmiAiRuntimeDescriptor descriptor;
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

const UmiAiPrivacyPolicy *umi_ai_authorengine_service_privacy_policy(
    const UmiAiAuthorEngineService *service)
{
    return service != NULL ? &service->privacy_policy : NULL;
}
