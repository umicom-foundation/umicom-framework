/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/authorengine_service.h
 *
 * PURPOSE:
 *   Coordinate the existing AI runtime with AuthorEngine configuration,
 *   runtime discovery, governed context plans, conversations and health.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * AuthorEngine stays independently executable.  This service owns integration
 * state and policy only; it neither copies AuthorEngine publishing logic nor
 * assumes that every provider uses the same transport.
 */
#ifndef UMICOM_AI_AUTHOR_ENGINE_SERVICE_H
#define UMICOM_AI_AUTHOR_ENGINE_SERVICE_H

#include "umicom/ai/authorengine.h"
#include "umicom/ai/context_broker.h"
#include "umicom/ai/conversation_archive.h"
#include "umicom/ai/runtime.h"
#include "umicom/ai/runtime_catalogue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai author engine service data shared with callers of this public contract.
 */
typedef struct UmiAiAuthorEngineService UmiAiAuthorEngineService;

/**
 * Represent the ai author engine service snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiAiAuthorEngineServiceSnapshot {
    uint64_t revision;
    size_t providers;
    size_t runtimes;
    size_t healthy_runtimes;
    size_t context_sources;
    size_t sessions;
    uint32_t context_limit;
    uint32_t reserved_output_tokens;
    int remote_allowed;
    int persistence_allowed;
    char active_session_id[UMI_AI_ID_CAPACITY];
    char executable[UMI_AI_TEXT_CAPACITY];
    char workspace[UMI_AI_TEXT_CAPACITY];
} UmiAiAuthorEngineServiceSnapshot;

/**
 * Initialise ai authorengine service from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_authorengine_service_create(
    UmiAiRuntime *runtime,
    const UmiAiAuthorEngineConfig *config,
    UmiAiAuthorEngineService **out_service);
/**
 * Release or reset state held by ai authorengine service so the same storage can be reused
 * safely.
 */
void umi_ai_authorengine_service_destroy(UmiAiAuthorEngineService *service);
/**
 * Provide the ai authorengine service catalogue operation used by this module and its
 * client applications.
 */
UmiAiRuntimeCatalogue *umi_ai_authorengine_service_catalogue(
    UmiAiAuthorEngineService *service);
/**
 * Provide the ai authorengine service context operation used by this module and its client
 * applications.
 */
UmiAiContextBroker *umi_ai_authorengine_service_context(
    UmiAiAuthorEngineService *service);
/**
 * Provide the ai authorengine service conversations operation used by this module and its
 * client applications.
 */
UmiAiConversationArchive *umi_ai_authorengine_service_conversations(
    UmiAiAuthorEngineService *service);
/**
 * Provide the ai authorengine service set policy operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_authorengine_service_set_policy(
    UmiAiAuthorEngineService *service,
    const UmiAiPolicy *provider_policy,
    const UmiAiPrivacyPolicy *privacy_policy,
    uint32_t context_limit,
    uint32_t reserved_output_tokens);
/**
 * Provide the ai authorengine service register runtime operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_authorengine_service_register_runtime(
    UmiAiAuthorEngineService *service,
    const UmiAiRuntimeDescriptor *descriptor);
/**
 * Provide the ai authorengine service begin session operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_authorengine_service_begin_session(
    UmiAiAuthorEngineService *service,
    const char *session_id,
    const char *provider_id,
    const char *model_id,
    const char *workspace_id,
    const char *title,
    uint64_t timestamp_ns);
/**
 * Provide the ai authorengine service add message operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_authorengine_service_add_message(
    UmiAiAuthorEngineService *service,
    const char *session_id,
    const UmiAiMessage *message,
    uint64_t timestamp_ns);
/**
 * Provide the ai authorengine service plan context operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_authorengine_service_plan_context(
    UmiAiAuthorEngineService *service,
    const char *runtime_id,
    int sensitive_approved,
    UmiAiContextPlan *out_plan);
/**
 * Provide the ai authorengine service refresh health operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_authorengine_service_refresh_health(
    UmiAiAuthorEngineService *service,
    uint64_t timestamp_ns,
    size_t *out_healthy_runtimes);
/**
 * Provide the ai authorengine service save session operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_authorengine_service_save_session(
    UmiAiAuthorEngineService *service,
    const char *session_id,
    const char *path);
/**
 * Provide the ai authorengine service load session operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_authorengine_service_load_session(
    UmiAiAuthorEngineService *service,
    const char *path,
    UmiAiConversationRecord *out_record);
/**
 * Provide the ai authorengine service snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_authorengine_service_snapshot(
    const UmiAiAuthorEngineService *service,
    UmiAiAuthorEngineServiceSnapshot *out_snapshot);
/**
 * Provide the ai authorengine service privacy policy operation used by this module and its
 * client applications.
 */
const UmiAiPrivacyPolicy *umi_ai_authorengine_service_privacy_policy(
    const UmiAiAuthorEngineService *service);

#ifdef __cplusplus
}
#endif

#endif
