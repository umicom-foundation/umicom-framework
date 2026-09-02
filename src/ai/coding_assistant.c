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
};

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

/*
 * Find ai coding assistant task while leaving the underlying catalogue or model owned by
 * this module.
 */
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

/*
 * Provide the ai coding assistant find patch operation used by this module and its client
 * applications.
 */
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

/*
 * Provide the ai coding assistant approve patch operation used by this module and its
 * client applications.
 */
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

/*
 * Provide the ai coding assistant reject patch operation used by this module and its
 * client applications.
 */
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

/*
 * Provide the ai coding assistant apply patch operation used by this module and its client
 * applications.
 */
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

/*
 * Provide the ai coding assistant revert patch operation used by this module and its
 * client applications.
 */
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

/*
 * Provide the ai coding assistant snapshot operation used by this module and its client
 * applications.
 */
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
