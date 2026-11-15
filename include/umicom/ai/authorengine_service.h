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
    /* Service-owned changes advance this counter, including each accepted
     * health row before a later failure. Direct child catalogue/context/archive
     * edits have independent identities and must be observed separately. */
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

/* Mutating service operations return CAPACITY_EXCEEDED before writing when
 * the service revision is exhausted. Message append also refuses an exhausted
 * conversation-record revision. Read-only snapshots/save/context planning remain
 * available; no operation resets or wraps an exhausted identity. */
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
/** Serialized health-refresh lifetime preflight. NULL is idle. BUSY retains
 * the complete service during either health API, including external probes.
 * This is not a lease or cross-thread synchronization. */
UmiStatus UmiAiAuthorEngineServiceCheckIdle(const UmiAiAuthorEngineService *service);
/** Refuse active health refresh before releasing any child. NULL is OK.
 * Clear the owner handle only after OK. Other callers and borrowed child
 * access still require external lifecycle serialization. */
UmiStatus UmiAiAuthorEngineServiceDestroyChecked(UmiAiAuthorEngineService *service);
/* The compatibility form retains a busy service but cannot report refusal. */
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
/* Identity of the owners used to prepare one context preview. Retain the same
 * live service alongside this value; numbers from another service can collide.
 * This is not an execution permit, file digest, model identity or durable token. */
typedef struct UmiAiContextPlanObservation {
    uint64_t service_revision;
    uint64_t context_revision;
    uint64_t runtime_revision;
    char runtime_id[UMI_AI_ID_CAPACITY];
} UmiAiContextPlanObservation;

/* Prepare the canonical context plan and its observation together. Both outputs
 * are required, disjoint, and unchanged on failure. Runtime ID may be borrowed
 * from the previous observation. Serialize service and child-owner access.
 * Any later service-owned change invalidates the observation conservatively;
 * direct context edits and changes to the selected runtime are detected too.
 * Direct changes to unrelated runtime rows do not invalidate this preview. */
UmiStatus UmiAiAuthorEnginePlanContextObserved(UmiAiAuthorEngineService *service,
    const char *runtime_id, int sensitive_approved, UmiAiContextPlan *out_plan,
    UmiAiContextPlanObservation *out_observation);
/* Read-only identity check: BUSY means service, broker or selected runtime has
 * changed/disappeared. Invalid observations return INVALID_ARGUMENT; invalid
 * owner state is reported by its validator. OK means these metadata identities
 * still match only. Recheck execution policy, request/workspace, availability,
 * actual source text, final token count and transport separately. The caller
 * must serialize checking and its dependent owner action; this API is no lock. */
UmiStatus UmiAiAuthorEngineCheckContextObservation(const UmiAiAuthorEngineService *service,
    const UmiAiContextPlanObservation *observation);

/**
 * Provide the ai authorengine service refresh health operation used by this module and its
 * client applications.
 */
/* Serialize service access and keep it and its runtime alive across callbacks.
 * Refresh visits the initial record count; callback additions wait until next
 * refresh. Replacement of a probed record returns BUSY without writing its old
 * health result. Earlier successful records stay updated; on failure the caller's
 * healthy count is unchanged. Reentry through either refresh API returns BUSY.
 * These provider-wide observations do not authenticate a model or bind a distinct
 * runtime endpoint; use the descriptor-aware API for endpoint-specific probes. */
UmiStatus umi_ai_authorengine_service_refresh_health(
    UmiAiAuthorEngineService *service,
    uint64_t timestamp_ns,
    size_t *out_healthy_runtimes);
/* The descriptor is a borrowed immutable snapshot valid only during the callback.
 * Use its endpoint/model/configuration to select an approved probe. The Framework
 * initializes health (including provider_id); return OK to publish an observation,
 * including unavailable. An error/cancellation stops without publishing that record.
 * The callback owns approval, transport, timeout and endpoint identity checks. */
typedef UmiStatus (*UmiAiRuntimeHealthProbeFn)(
    void *context, const UmiAiRuntimeDescriptor *descriptor, UmiAiHealth *health);

/* Refresh each initially present runtime through the explicit descriptor-aware
 * probe, without consulting the provider-wide health callback. The callback and
 * output count are required. Context may be NULL when the callback needs no state.
 * Earlier successful writes remain on a later failure; count is unchanged then.
 * Reentry through either refresh API returns BUSY before callbacks or mutation.
 * Serialize all service access; this guard is not a cross-thread lock. Keep the
 * service, runtime and context alive, and defer destruction until refresh returns.
 * A published observation still does not authenticate a model or child process. */
UmiStatus UmiAiAuthorEngineRefreshRuntimeHealth(
    UmiAiAuthorEngineService *service,
    uint64_t timestamp_ns,
    UmiAiRuntimeHealthProbeFn probe,
    void *context,
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
/* Historical file identity and current stored identity are different. Import
 * assigns a fresh archive revision and advances the coordinating service once. */
typedef struct UmiAiConversationImportReport {
    char session_id[UMI_AI_ID_CAPACITY];
    uint64_t source_record_revision;
    uint64_t stored_record_revision;
    uint64_t service_revision;
    int replaced;
} UmiAiConversationImportReport;

/* Adopt a complete in-memory record. Expected zero inserts only if absent;
 * a nonzero expected revision explicitly replaces that same archive record.
 * BUSY means the selected record no longer has the expected identity. All
 * refusals preserve archive, active session, service revision and output report.
 * The required report must not overlap the source record or service storage.
 * Input remains caller-owned. No model is registered, loaded or invoked; saved
 * provider metadata may be unavailable. Import does not relax service policy.
 * Serialize service/child access; there is no cross-thread lock or file write. */
UmiStatus UmiAiAuthorEngineImportSession(UmiAiAuthorEngineService *service,
    const UmiAiConversationRecord *record, uint64_t expected_record_revision,
    UmiAiConversationImportReport *out_report);
/* Compose bounded absolute regular-file reading with conditional import.
 * maximum_bytes is in 1..UMI_AI_CONVERSATION_FILE_MAX_BYTES. Read/parse/conflict
 * failure preserves current state and report. File I/O can block; an interactive
 * owner should parse on an owned worker, then call ImportSession on its owner
 * thread after checking the initiating request. This convenience call assumes
 * serialized ownership for its full duration and performs no model/network work.
 * Legacy load_session remains available with its compatibility reader and
 * historical parsed-record output; this report explicitly identifies stored state. */
UmiStatus UmiAiAuthorEngineLoadSessionBounded(UmiAiAuthorEngineService *service,
    const char *absolute_path, size_t maximum_bytes, uint64_t expected_record_revision,
    UmiAiConversationImportReport *out_report);

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
