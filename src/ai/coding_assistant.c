/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/coding_assistant.c
 *
 * PURPOSE:
 *   Join AuthorEngine context governance, repository planning, task history and
 *   reviewable patch transactions behind one reusable service boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Preparing a task does not execute a model or edit a file.  It creates an
 * auditable plan that a provider adapter can consume later.  Patch mutation is
 * a separate, explicitly approved operation using callbacks owned by the host.
 */
#include "umicom/ai/coding_assistant.h"
#include "../base/value_construction_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiAiCodingAssistantService {
    UmiAiAuthorEngineService *authorengine;
    UmiAiCodingContextIndex *context;
    UmiAiCodingPatchPolicy patch_policy;
    UmiAiCodingTaskPlan tasks[UMI_AI_CODING_TASK_HISTORY_CAPACITY];
    size_t task_count;
    UmiAiCodingPatch patches[UMI_AI_CODING_PATCH_HISTORY_CAPACITY];
    size_t patch_count;
    uint64_t revision;
    int patch_transaction_active;
};

/* Every service operation shares one bounded owner admission path. Child
 * context revisions and patch revisions have independent identities. */
static UmiStatus coding_owner_validate(const UmiAiCodingAssistantService *service)
{
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (service->authorengine == NULL || service->context == NULL ||
        service->revision == 0U ||
        service->task_count > UMI_AI_CODING_TASK_HISTORY_CAPACITY ||
        service->patch_count > UMI_AI_CODING_PATCH_HISTORY_CAPACITY ||
        UmiAiCodingPatchPolicyValidate(&service->patch_policy) != UMI_STATUS_OK ||
        UmiAiCodingContextIndexValidate(service->context) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < service->task_count; ++i) {
        const UmiAiCodingTaskPlan *task = &service->tasks[i];
        if (umi_ai_coding_request_validate(&task->request) != UMI_STATUS_OK ||
            task->revision == 0U || task->revision > service->revision ||
            task->repository_context.file_count > UMI_AI_CODING_CONTEXT_PLAN_MAX ||
            task->general_context_sources > UMI_AI_CONTEXT_PLAN_SOURCE_MAX || !task->ready)
            return UMI_STATUS_INVALID_STATE;
    }
    for (size_t i = 0U; i < service->patch_count; ++i) {
        if (UmiAiCodingPatchCheckIntegrity(&service->patches[i]) != UMI_STATUS_OK ||
            service->patches[i].file_count == 0U) return UMI_STATUS_INVALID_STATE;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(service->patches[i].patch_id, service->patches[j].patch_id) == 0)
                return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

static UmiStatus coding_owner_mutable(const UmiAiCodingAssistantService *service)
{
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* This is a same-thread callback guard, not a concurrency primitive. */
    if (service->patch_transaction_active) return UMI_STATUS_BUSY;
    UmiStatus status = coding_owner_validate(service);
    if (status != UMI_STATUS_OK) return status;
    return service->revision == UINT64_MAX ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

static int coding_patch_query_valid(const char *patch_id)
{
    if (patch_id == NULL) return 0;
    for (size_t i = 0U; i < UMI_AI_ID_CAPACITY; ++i)
        if (patch_id[i] == '\0') return 1;
    return 0;
}

/* Parent composition roots can refuse teardown before freeing dependencies.
 * This is a serialized observation, not a lifetime reservation or a lock. */
UmiStatus UmiAiCodingAssistantCheckIdle(const UmiAiCodingAssistantService *service)
{
    return service != NULL && service->patch_transaction_active ? UMI_STATUS_BUSY : UMI_STATUS_OK;
}

UmiStatus UmiAiCodingAssistantDestroyChecked(UmiAiCodingAssistantService *service)
{
    if (service == NULL) return UMI_STATUS_OK;
    if (service->patch_transaction_active) return UMI_STATUS_BUSY;
    /* Destruction must remain available even when inspection rejects a
     * damaged metadata count; it never traverses task or patch arrays. */
    umi_ai_coding_context_index_destroy(service->context);
    free(service);
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
 * Initialise ai coding assistant from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ai_coding_assistant_create(
    UmiAiAuthorEngineService *authorengine,
    UmiAiCodingAssistantService **out_service)
{
    UmiAiCodingAssistantService *service;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (authorengine == NULL || out_service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_service = NULL;
    service = (UmiAiCodingAssistantService *)calloc(1U, sizeof(*service));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    service->authorengine = authorengine;
    service->patch_policy = umi_ai_coding_patch_policy_default();
    service->revision = 1U;
    status = umi_ai_coding_context_index_create(&service->context);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(service);
        return status;
    }
    *out_service = service;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ai coding assistant so the same storage can be reused
 * safely.
 */
/* Checked destruction refuses callback reentry without freeing the owner used by the outer patch operation. The former implementation is retained for owner review. */
#if 0
void umi_ai_coding_assistant_destroy(UmiAiCodingAssistantService *service)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return;
    umi_ai_coding_context_index_destroy(service->context);
    free(service);
}

#endif
void umi_ai_coding_assistant_destroy(UmiAiCodingAssistantService *service)
{
    /* Keep the legacy void API; a busy callback cannot destroy its live owner.
     * Hosts needing a result should use the checked call and retry after return. */
    (void)UmiAiCodingAssistantDestroyChecked(service);
}

/*
 * Provide the ai coding assistant context operation used by this module and its client
 * applications.
 */
UmiAiCodingContextIndex *umi_ai_coding_assistant_context(
    UmiAiCodingAssistantService *service)
{
    return service != NULL ? service->context : NULL;
}

/*
 * Provide the ai coding assistant set patch policy operation used by this module and its
 * client applications.
 */
/* Reuse Framework patch-policy admission and reserve a service revision before publishing a new policy. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_assistant_set_patch_policy(
    UmiAiCodingAssistantService *service,
    const UmiAiCodingPatchPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || policy == NULL || policy->maximum_files == 0U ||
        policy->maximum_files > UMI_AI_CODING_PATCH_FILE_MAX ||
        policy->maximum_changed_lines == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    service->patch_policy = *policy;
    ++service->revision;
    return UMI_STATUS_OK;
}

#endif
UmiStatus umi_ai_coding_assistant_set_patch_policy(
    UmiAiCodingAssistantService *service, const UmiAiCodingPatchPolicy *policy)
{
    UmiStatus status = coding_owner_mutable(service);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiCodingPatchPolicyValidate(policy);
    if (status != UMI_STATUS_OK) return status;
    service->patch_policy = *policy; ++service->revision;
    return UMI_STATUS_OK;
}

/* Provide the remember task operation used by this module and its client applications. */
static void remember_task(UmiAiCodingAssistantService *service,
                          const UmiAiCodingTaskPlan *plan)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (service->task_count == UMI_AI_CODING_TASK_HISTORY_CAPACITY) {
        (void)memmove(&service->tasks[0], &service->tasks[1],
                      (service->task_count - 1U) * sizeof(service->tasks[0]));
        --service->task_count;
    }
    service->tasks[service->task_count++] = *plan;
}

/*
 * Provide the ai coding assistant prepare operation used by this module and its client
 * applications.
 */
/* Use one catalogue provider identity for request and context privacy, then publish a complete task only after planning succeeds. This preserves output and history on refusal. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_assistant_prepare(
    UmiAiCodingAssistantService *service,
    const UmiAiCodingRequest *request,
    UmiAiProviderKind provider_kind,
    UmiAiCodingTaskPlan *out_plan)
{
    UmiAiContextPlan general;
    const UmiAiPrivacyPolicy *privacy;
    UmiStatus status;
    uint64_t hash;
    uint32_t repository_budget;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ai_coding_request_validate(request);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_authorengine_service_plan_context(
        service->authorengine, request->runtime_id,
        request->sensitive_approved, &general);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    privacy = umi_ai_authorengine_service_privacy_policy(service->authorengine);
    (void)memset(out_plan, 0, sizeof(*out_plan));
    out_plan->request = *request;
    repository_budget = request->context_token_budget < general.remaining_tokens
        ? request->context_token_budget : general.remaining_tokens;
    status = umi_ai_coding_context_plan(
        service->context, repository_budget,
        request->maximum_context_files, provider_kind, privacy,
        request->sensitive_approved, &out_plan->repository_context);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_plan->general_context_sources = general.source_count;
    out_plan->general_context_tokens = general.selected_tokens;
    out_plan->total_context_tokens = general.selected_tokens +
                                     out_plan->repository_context.selected_tokens;
    hash = umi_ai_coding_text_hash(request->instruction,
                                   strlen(request->instruction));
    hash ^= umi_ai_coding_text_hash(request->active_path,
                                    strlen(request->active_path));
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < out_plan->repository_context.file_count; ++index) {
        const char *path = out_plan->repository_context.files[index].path;
        hash ^= umi_ai_coding_text_hash(path, strlen(path));
        hash *= UINT64_C(1099511628211);
    }
    out_plan->plan_hash = hash;
    out_plan->ready = 1;
    out_plan->revision = ++service->revision;
    remember_task(service, out_plan);
    return UMI_STATUS_OK;
}

#endif
/* Share complete owner and callback-reentry admission with the other service mutations before staged task planning. The former implementation is retained for owner review. */
#if 0
static UmiStatus coding_assistant_prepare_build(
    UmiAiCodingAssistantService *service,
    const UmiAiCodingRequest *request,
    UmiAiProviderKind provider_kind,
    UmiAiCodingTaskPlan *out_plan)
{
    UmiAiContextPlan general;
    UmiAiRuntimeDescriptor selected_runtime;
    const UmiAiPrivacyPolicy *privacy;
    UmiStatus status;
    uint64_t hash;
    uint32_t repository_budget;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Bound the owner fields used by history publication before traversal. */
    if (service->revision == 0U || service->task_count > UMI_AI_CODING_TASK_HISTORY_CAPACITY ||
        service->patch_count > UMI_AI_CODING_PATCH_HISTORY_CAPACITY ||
        service->authorengine == NULL || service->context == NULL) return UMI_STATUS_INVALID_STATE;
    if (service->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (provider_kind < UMI_AI_PROVIDER_LOCAL || provider_kind > UMI_AI_PROVIDER_TEST)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ai_coding_request_validate(request);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* The catalogue is authoritative for both context paths. A caller may
     * assert the intended kind, but cannot relabel a remote runtime as local. */
    status = umi_ai_runtime_catalogue_find(umi_ai_authorengine_service_catalogue(service->authorengine),
        request->runtime_id, &selected_runtime);
    if (status != UMI_STATUS_OK) return status;
    if (provider_kind != selected_runtime.provider_kind) return UMI_STATUS_PERMISSION_DENIED;
    privacy = umi_ai_authorengine_service_privacy_policy(service->authorengine);
    status = umi_ai_privacy_policy_check_share(privacy, selected_runtime.provider_kind,
        request->classification, request->sensitive_approved);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_authorengine_service_plan_context(
        service->authorengine, request->runtime_id,
        request->sensitive_approved, &general);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    privacy = umi_ai_authorengine_service_privacy_policy(service->authorengine);
    (void)memset(out_plan, 0, sizeof(*out_plan));
    out_plan->request = *request;
    repository_budget = request->context_token_budget < general.remaining_tokens
        ? request->context_token_budget : general.remaining_tokens;
    status = umi_ai_coding_context_plan(
        service->context, repository_budget,
        request->maximum_context_files, selected_runtime.provider_kind, privacy,
        request->sensitive_approved, &out_plan->repository_context);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_plan->general_context_sources = general.source_count;
    out_plan->general_context_tokens = general.selected_tokens;
    out_plan->total_context_tokens = general.selected_tokens +
                                     out_plan->repository_context.selected_tokens;
    hash = umi_ai_coding_text_hash(request->instruction,
                                   strlen(request->instruction));
    hash ^= umi_ai_coding_text_hash(request->active_path,
                                    strlen(request->active_path));
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < out_plan->repository_context.file_count; ++index) {
        const char *path = out_plan->repository_context.files[index].path;
        hash ^= umi_ai_coding_text_hash(path, strlen(path));
        hash *= UINT64_C(1099511628211);
    }
    out_plan->plan_hash = hash;
    out_plan->ready = 1;
    out_plan->revision = ++service->revision;
    remember_task(service, out_plan);
    return UMI_STATUS_OK;
}

#endif
static UmiStatus coding_assistant_prepare_build(
    UmiAiCodingAssistantService *service,
    const UmiAiCodingRequest *request,
    UmiAiProviderKind provider_kind,
    UmiAiCodingTaskPlan *out_plan)
{
    UmiAiContextPlan general;
    UmiAiRuntimeDescriptor selected_runtime;
    const UmiAiPrivacyPolicy *privacy;
    UmiStatus status;
    uint64_t hash;
    uint32_t repository_budget;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = coding_owner_mutable(service);
    if (status != UMI_STATUS_OK) return status;
    if (provider_kind < UMI_AI_PROVIDER_LOCAL || provider_kind > UMI_AI_PROVIDER_TEST)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ai_coding_request_validate(request);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* The catalogue is authoritative for both context paths. A caller may
     * assert the intended kind, but cannot relabel a remote runtime as local. */
    status = umi_ai_runtime_catalogue_find(umi_ai_authorengine_service_catalogue(service->authorengine),
        request->runtime_id, &selected_runtime);
    if (status != UMI_STATUS_OK) return status;
    if (provider_kind != selected_runtime.provider_kind) return UMI_STATUS_PERMISSION_DENIED;
    privacy = umi_ai_authorengine_service_privacy_policy(service->authorengine);
    status = umi_ai_privacy_policy_check_share(privacy, selected_runtime.provider_kind,
        request->classification, request->sensitive_approved);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_authorengine_service_plan_context(
        service->authorengine, request->runtime_id,
        request->sensitive_approved, &general);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    privacy = umi_ai_authorengine_service_privacy_policy(service->authorengine);
    (void)memset(out_plan, 0, sizeof(*out_plan));
    out_plan->request = *request;
    repository_budget = request->context_token_budget < general.remaining_tokens
        ? request->context_token_budget : general.remaining_tokens;
    status = umi_ai_coding_context_plan(
        service->context, repository_budget,
        request->maximum_context_files, selected_runtime.provider_kind, privacy,
        request->sensitive_approved, &out_plan->repository_context);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_plan->general_context_sources = general.source_count;
    out_plan->general_context_tokens = general.selected_tokens;
    out_plan->total_context_tokens = general.selected_tokens +
                                     out_plan->repository_context.selected_tokens;
    hash = umi_ai_coding_text_hash(request->instruction,
                                   strlen(request->instruction));
    hash ^= umi_ai_coding_text_hash(request->active_path,
                                    strlen(request->active_path));
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < out_plan->repository_context.file_count; ++index) {
        const char *path = out_plan->repository_context.files[index].path;
        hash ^= umi_ai_coding_text_hash(path, strlen(path));
        hash *= UINT64_C(1099511628211);
    }
    out_plan->plan_hash = hash;
    out_plan->ready = 1;
    out_plan->revision = ++service->revision;
    remember_task(service, out_plan);
    return UMI_STATUS_OK;
}

/* Only the candidate is exposed to fallible planning. The accepted history
 * and next revision are assigned at the end of the builder; publication then
 * copies the complete value without further fallible operations. */
UMI_DEFINE_STAGED_VALUE_INIT(umi_ai_coding_assistant_prepare, UmiAiCodingTaskPlan, out_plan,
    (UmiAiCodingAssistantService *service, const UmiAiCodingRequest *request,
     UmiAiProviderKind provider_kind, UmiAiCodingTaskPlan *out_plan),
    coding_assistant_prepare_build(service, request, provider_kind, candidate))

/*
 * Find ai coding assistant task while leaving the underlying catalogue or model owned by
 * this module.
 */
/* Admit owner counts and stored text before read-only traversal; rejected reads leave the destination unchanged, including during a callback. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_assistant_task_at(
    const UmiAiCodingAssistantService *service,
    size_t position,
    UmiAiCodingTaskPlan *out_plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= service->task_count) return UMI_STATUS_NOT_FOUND;
    *out_plan = service->tasks[position];
    return UMI_STATUS_OK;
}

#endif
UmiStatus umi_ai_coding_assistant_task_at(
    const UmiAiCodingAssistantService *service,
    size_t position,
    UmiAiCodingTaskPlan *out_plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus admission = coding_owner_validate(service);
    if (admission != UMI_STATUS_OK) return admission;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= service->task_count) return UMI_STATUS_NOT_FOUND;
    *out_plan = service->tasks[position];
    return UMI_STATUS_OK;
}

/* Provide the find patch operation used by this module and its client applications. */
static size_t find_patch(const UmiAiCodingAssistantService *service,
                         const char *patch_id)
{
    size_t position;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < service->patch_count; ++position) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(service->patches[position].patch_id, patch_id) == 0) {
            return position;
        }
    }
    return service->patch_count;
}

/*
 * Provide the ai coding assistant record patch operation used by this module and its
 * client applications.
 */
/* Reserve owner mutation capacity before replacing or rolling patch history; reject reentry while a patch callback uses the current array. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_assistant_record_patch(
    UmiAiCodingAssistantService *service,
    const UmiAiCodingPatch *patch)
{
    size_t position;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || patch == NULL ||
        patch->state != UMI_AI_CODING_PATCH_DRAFT) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_ai_coding_patch_validate(patch, &service->patch_policy);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    position = find_patch(service, patch->patch_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (position == service->patch_count) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (service->patch_count == UMI_AI_CODING_PATCH_HISTORY_CAPACITY) {
            (void)memmove(&service->patches[0], &service->patches[1],
                          (service->patch_count - 1U) *
                              sizeof(service->patches[0]));
            --service->patch_count;
            position = service->patch_count;
        }
        ++service->patch_count;
    }
    service->patches[position] = *patch;
    ++service->revision;
    return UMI_STATUS_OK;
}

#endif
UmiStatus umi_ai_coding_assistant_record_patch(
    UmiAiCodingAssistantService *service,
    const UmiAiCodingPatch *patch)
{
    size_t position;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || patch == NULL ||
        patch->state != UMI_AI_CODING_PATCH_DRAFT) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = coding_owner_mutable(service);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_coding_patch_validate(patch, &service->patch_policy);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    position = find_patch(service, patch->patch_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (position == service->patch_count) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (service->patch_count == UMI_AI_CODING_PATCH_HISTORY_CAPACITY) {
            (void)memmove(&service->patches[0], &service->patches[1],
                          (service->patch_count - 1U) *
                              sizeof(service->patches[0]));
            --service->patch_count;
            position = service->patch_count;
        }
        ++service->patch_count;
    }
    service->patches[position] = *patch;
    ++service->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the ai coding assistant find patch operation used by this module and its client
 * applications.
 */
/* Admit owner counts and stored text before read-only traversal; rejected reads leave the destination unchanged, including during a callback. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_assistant_find_patch(
    const UmiAiCodingAssistantService *service,
    const char *patch_id,
    UmiAiCodingPatch *out_patch)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || patch_id == NULL || out_patch == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_patch(service, patch_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == service->patch_count) return UMI_STATUS_NOT_FOUND;
    *out_patch = service->patches[position];
    return UMI_STATUS_OK;
}

#endif
UmiStatus umi_ai_coding_assistant_find_patch(
    const UmiAiCodingAssistantService *service,
    const char *patch_id,
    UmiAiCodingPatch *out_patch)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!coding_patch_query_valid(patch_id) || out_patch == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiStatus admission = coding_owner_validate(service);
    if (admission != UMI_STATUS_OK) return admission;
    position = find_patch(service, patch_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == service->patch_count) return UMI_STATUS_NOT_FOUND;
    *out_patch = service->patches[position];
    return UMI_STATUS_OK;
}

/*
 * Provide the ai coding assistant approve patch operation used by this module and its
 * client applications.
 */
/* Bound the patch query and owner before mutation. Callback-driven operations guard their owner until completion so nested service edits cannot alter the active patch or consume its reserved revision. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_assistant_approve_patch(
    UmiAiCodingAssistantService *service,
    const char *patch_id,
    const char *approved_by)
{
    size_t position;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || patch_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    position = find_patch(service, patch_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == service->patch_count) return UMI_STATUS_NOT_FOUND;
    status = umi_ai_coding_patch_approve(&service->patches[position], approved_by);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}

#endif
UmiStatus umi_ai_coding_assistant_approve_patch(
    UmiAiCodingAssistantService *service,
    const char *patch_id,
    const char *approved_by)
{
    size_t position;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!coding_patch_query_valid(patch_id)) return UMI_STATUS_INVALID_ARGUMENT;
    status = coding_owner_mutable(service);
    if (status != UMI_STATUS_OK) return status;
    position = find_patch(service, patch_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == service->patch_count) return UMI_STATUS_NOT_FOUND;
    status = umi_ai_coding_patch_approve(&service->patches[position], approved_by);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}

/*
 * Provide the ai coding assistant reject patch operation used by this module and its
 * client applications.
 */
/* Bound the patch query and owner before mutation. Callback-driven operations guard their owner until completion so nested service edits cannot alter the active patch or consume its reserved revision. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_assistant_reject_patch(
    UmiAiCodingAssistantService *service,
    const char *patch_id)
{
    size_t position;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || patch_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    position = find_patch(service, patch_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == service->patch_count) return UMI_STATUS_NOT_FOUND;
    status = umi_ai_coding_patch_reject(&service->patches[position]);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}

#endif
UmiStatus umi_ai_coding_assistant_reject_patch(
    UmiAiCodingAssistantService *service,
    const char *patch_id)
{
    size_t position;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!coding_patch_query_valid(patch_id)) return UMI_STATUS_INVALID_ARGUMENT;
    status = coding_owner_mutable(service);
    if (status != UMI_STATUS_OK) return status;
    position = find_patch(service, patch_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == service->patch_count) return UMI_STATUS_NOT_FOUND;
    status = umi_ai_coding_patch_reject(&service->patches[position]);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}

/*
 * Provide the ai coding assistant apply patch operation used by this module and its client
 * applications.
 */
/* Bound the patch query and owner before mutation. Callback-driven operations guard their owner until completion so nested service edits cannot alter the active patch or consume its reserved revision. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_assistant_apply_patch(
    UmiAiCodingAssistantService *service,
    const char *patch_id,
    const UmiAiCodingFileAdapter *adapter)
{
    size_t position;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || patch_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    position = find_patch(service, patch_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == service->patch_count) return UMI_STATUS_NOT_FOUND;
    status = umi_ai_coding_patch_apply(&service->patches[position],
                                       &service->patch_policy, adapter);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}

#endif
/* Detailed patch reports now cross the shared assistant boundary, and attempted mutation revisions remain visible after failure. The legacy call delegates while preserving callback ownership and primary return status. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_assistant_apply_patch(
    UmiAiCodingAssistantService *service,
    const char *patch_id,
    const UmiAiCodingFileAdapter *adapter)
{
    size_t position;
    UmiStatus status;
    UmiAiCodingFileAdapter stable_adapter;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!coding_patch_query_valid(patch_id)) return UMI_STATUS_INVALID_ARGUMENT;
    status = coding_owner_mutable(service);
    if (status != UMI_STATUS_OK) return status;
    position = find_patch(service, patch_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == service->patch_count) return UMI_STATUS_NOT_FOUND;
    status = UmiAiCodingFileAdapterValidate(adapter);
    if (status != UMI_STATUS_OK) return status;
    /* Snapshot callback slots and block same-owner mutation/destruction
     * until the entire forward/compensation operation returns normally. */
    if (adapter != NULL) stable_adapter = *adapter;
    service->patch_transaction_active = 1;
    status = umi_ai_coding_patch_apply(&service->patches[position],
                                       &service->patch_policy, adapter != NULL ? &stable_adapter : NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    service->patch_transaction_active = 0;
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}

#endif
UmiStatus umi_ai_coding_assistant_apply_patch(
    UmiAiCodingAssistantService *service, const char *patch_id,
    const UmiAiCodingFileAdapter *adapter)
{
    UmiAiCodingPatchTransactionReport report = {0};
    return UmiAiCodingAssistantApplyPatchWithReport(service, patch_id, adapter, &report);
}

/*
 * Provide the ai coding assistant revert patch operation used by this module and its
 * client applications.
 */
/* Bound the patch query and owner before mutation. Callback-driven operations guard their owner until completion so nested service edits cannot alter the active patch or consume its reserved revision. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_assistant_revert_patch(
    UmiAiCodingAssistantService *service,
    const char *patch_id,
    const UmiAiCodingFileAdapter *adapter)
{
    size_t position;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || patch_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    position = find_patch(service, patch_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == service->patch_count) return UMI_STATUS_NOT_FOUND;
    status = umi_ai_coding_patch_revert(&service->patches[position], adapter);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}

#endif
/* Detailed patch reports now cross the shared assistant boundary, and attempted mutation revisions remain visible after failure. The legacy call delegates while preserving callback ownership and primary return status. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_assistant_revert_patch(
    UmiAiCodingAssistantService *service,
    const char *patch_id,
    const UmiAiCodingFileAdapter *adapter)
{
    size_t position;
    UmiStatus status;
    UmiAiCodingFileAdapter stable_adapter;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!coding_patch_query_valid(patch_id)) return UMI_STATUS_INVALID_ARGUMENT;
    status = coding_owner_mutable(service);
    if (status != UMI_STATUS_OK) return status;
    position = find_patch(service, patch_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == service->patch_count) return UMI_STATUS_NOT_FOUND;
    status = UmiAiCodingFileAdapterValidate(adapter);
    if (status != UMI_STATUS_OK) return status;
    /* Snapshot callback slots and block same-owner mutation/destruction
     * until the entire forward/compensation operation returns normally. */
    if (adapter != NULL) stable_adapter = *adapter;
    service->patch_transaction_active = 1;
    status = umi_ai_coding_patch_revert(&service->patches[position], adapter != NULL ? &stable_adapter : NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    service->patch_transaction_active = 0;
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}

#endif
UmiStatus umi_ai_coding_assistant_revert_patch(
    UmiAiCodingAssistantService *service, const char *patch_id,
    const UmiAiCodingFileAdapter *adapter)
{
    UmiAiCodingPatchTransactionReport report = {0};
    return UmiAiCodingAssistantRevertPatchWithReport(service, patch_id, adapter, &report);
}

/*
 * Provide the ai coding assistant snapshot operation used by this module and its client
 * applications.
 */
/* Admit owner counts and stored text before read-only traversal; rejected reads leave the destination unchanged, including during a callback. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_assistant_snapshot(
    const UmiAiCodingAssistantService *service,
    UmiAiCodingAssistantSnapshot *out_snapshot)
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
    out_snapshot->repository_files = umi_ai_coding_context_count(service->context);
    out_snapshot->task_count = service->task_count;
    out_snapshot->patch_count = service->patch_count;
    out_snapshot->patch_policy = service->patch_policy;
    /* Apply this branch only when its contract condition is satisfied. */
    if (service->task_count > 0U) {
        (void)copy_text(out_snapshot->last_request_id,
                        sizeof(out_snapshot->last_request_id),
                        service->tasks[service->task_count - 1U]
                            .request.request_id);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (service->patch_count > 0U) {
        (void)copy_text(out_snapshot->last_patch_id,
                        sizeof(out_snapshot->last_patch_id),
                        service->patches[service->patch_count - 1U].patch_id);
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < service->patch_count; ++index) {
        /* Select the behaviour associated with the requested command or state value. */
        switch (service->patches[index].state) {
            case UMI_AI_CODING_PATCH_DRAFT: ++out_snapshot->draft_patches; break;
            case UMI_AI_CODING_PATCH_APPROVED:
                ++out_snapshot->approved_patches;
                break;
            case UMI_AI_CODING_PATCH_APPLIED: ++out_snapshot->applied_patches; break;
            case UMI_AI_CODING_PATCH_REVERTED:
                ++out_snapshot->reverted_patches;
                break;
            default: break;
        }
    }
    return UMI_STATUS_OK;
}

#endif
UmiStatus umi_ai_coding_assistant_snapshot(
    const UmiAiCodingAssistantService *service,
    UmiAiCodingAssistantSnapshot *out_snapshot)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiStatus admission = coding_owner_validate(service);
    if (admission != UMI_STATUS_OK) return admission;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->revision = service->revision;
    out_snapshot->repository_files = umi_ai_coding_context_count(service->context);
    out_snapshot->task_count = service->task_count;
    out_snapshot->patch_count = service->patch_count;
    out_snapshot->patch_policy = service->patch_policy;
    /* Apply this branch only when its contract condition is satisfied. */
    if (service->task_count > 0U) {
        (void)copy_text(out_snapshot->last_request_id,
                        sizeof(out_snapshot->last_request_id),
                        service->tasks[service->task_count - 1U]
                            .request.request_id);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (service->patch_count > 0U) {
        (void)copy_text(out_snapshot->last_patch_id,
                        sizeof(out_snapshot->last_patch_id),
                        service->patches[service->patch_count - 1U].patch_id);
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < service->patch_count; ++index) {
        /* Select the behaviour associated with the requested command or state value. */
        switch (service->patches[index].state) {
            case UMI_AI_CODING_PATCH_DRAFT: ++out_snapshot->draft_patches; break;
            case UMI_AI_CODING_PATCH_APPROVED:
                ++out_snapshot->approved_patches;
                break;
            case UMI_AI_CODING_PATCH_APPLIED: ++out_snapshot->applied_patches; break;
            case UMI_AI_CODING_PATCH_REVERTED:
                ++out_snapshot->reverted_patches;
                break;
            default: break;
        }
    }
    return UMI_STATUS_OK;
}

/* A service revision follows changes to its owned patch even when the primary
 * mutation fails. Reserve headroom before callbacks and publish a report only
 * if the patch layer admitted the operation. */
static UmiStatus coding_assistant_patch_transaction(UmiAiCodingAssistantService *service,
    const char *patch_id, const UmiAiCodingFileAdapter *adapter, int reverting,
    UmiAiCodingPatchTransactionReport *outReport)
{
    if (outReport == NULL || !coding_patch_query_valid(patch_id)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = coding_owner_mutable(service);
    if (status != UMI_STATUS_OK) return status;
    size_t position = find_patch(service, patch_id);
    if (position == service->patch_count) return UMI_STATUS_NOT_FOUND;
    status = UmiAiCodingFileAdapterValidate(adapter);
    if (status != UMI_STATUS_OK) return status;
    UmiAiCodingFileAdapter stable_adapter = *adapter;
    UmiAiCodingPatchTransactionReport report = {0};
    service->patch_transaction_active = 1;
    status = reverting ? UmiAiCodingPatchRevertWithReport(&service->patches[position], &stable_adapter, &report) :
        UmiAiCodingPatchApplyWithReport(&service->patches[position], &service->patch_policy, &stable_adapter, &report);
    service->patch_transaction_active = 0;
    if (report.admitted) {
        if (report.revision_after != report.revision_before) ++service->revision;
        *outReport = report;
    }
    return status;
}
UmiStatus UmiAiCodingAssistantApplyPatchWithReport(UmiAiCodingAssistantService *service,
    const char *patch_id, const UmiAiCodingFileAdapter *adapter,
    UmiAiCodingPatchTransactionReport *outReport)
{
    return coding_assistant_patch_transaction(service, patch_id, adapter, 0, outReport);
}
UmiStatus UmiAiCodingAssistantRevertPatchWithReport(UmiAiCodingAssistantService *service,
    const char *patch_id, const UmiAiCodingFileAdapter *adapter,
    UmiAiCodingPatchTransactionReport *outReport)
{
    return coding_assistant_patch_transaction(service, patch_id, adapter, 1, outReport);
}
