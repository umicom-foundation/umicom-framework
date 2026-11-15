/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/context_materializer.c
 *
 * PURPOSE:
 *   Read planned repository files through the controlled workspace adapter and
 *   retain bounded owned text for provider prompt construction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/context_materializer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Framework now admits metadata and successful callback lengths before reading
 * content, and stages publication so failures preserve the caller value. The
 * former materializer and unchecked lookup remain here for owner review. */
#if 0
/*
 * Provide the ai coding materialize context operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_materialize_context(
    const UmiAiCodingTaskPlan *plan,
    const UmiAiCodingWorkspaceAdapter *workspace,
    UmiAiCodingMaterializedContext *out_context)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || workspace == NULL || out_context == NULL ||
        !plan->ready) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_coding_workspace_adapter_validate(workspace);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)memset(out_context, 0, sizeof(*out_context));
    out_context->revision = plan->revision;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < plan->repository_context.file_count;
         ++index) {
        const UmiAiCodingContextFile *source =
            &plan->repository_context.files[index];
        UmiAiCodingMaterializedFile *target;
        size_t length = 0U;

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (out_context->file_count >=
            UMI_AI_CODING_RUNTIME_CONTEXT_CAPACITY) {
            out_context->truncated = 1;
            out_context->skipped_count +=
                plan->repository_context.file_count - index;
            break;
        }

        target = &out_context->files[out_context->file_count];
        (void)memset(target, 0, sizeof(*target));

        status = workspace->read(
            workspace->user_data,
            source->path,
            target->text,
            sizeof(target->text),
            &length);

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_NOT_FOUND ||
            status == UMI_STATUS_CAPACITY_EXCEEDED ||
            status == UMI_STATUS_PARSE_ERROR) {
            out_context->skipped_count += 1U;
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_CAPACITY_EXCEEDED) {
                out_context->truncated = 1;
            }
            continue;
        }

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        (void)snprintf(
            target->path, sizeof(target->path), "%s", source->path);
        (void)snprintf(
            target->language_id, sizeof(target->language_id),
            "%s", source->language_id);
        target->length = length;
        target->hash = umi_ai_coding_text_hash(target->text, length);
        target->estimated_tokens = source->estimated_tokens;
        target->active =
            plan->request.active_path[0] != '\0' &&
            strcmp(source->path, plan->request.active_path) == 0;

        out_context->estimated_tokens += source->estimated_tokens;
        out_context->file_count += 1U;
    }

    return UMI_STATUS_OK;
}

/*
 * Find ai coding materialized context while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_ai_coding_materialized_context_find(
    const UmiAiCodingMaterializedContext *context,
    const char *path,
    UmiAiCodingMaterializedFile *out_file)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || path == NULL || out_file == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < context->file_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(context->files[index].path, path) == 0) {
            *out_file = context->files[index];
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_NOT_FOUND;
}

#endif

static_assert(UMI_AI_CODING_RUNTIME_CONTEXT_CAPACITY <= UMI_AI_CODING_CONTEXT_PLAN_MAX, "Materialized context must fit the planned selection");
static_assert(sizeof(((UmiAiCodingMaterializedFile *)0)->path) == sizeof(((UmiAiCodingContextFile *)0)->path), "Owned and selected paths share capacity");
static_assert(sizeof(((UmiAiCodingMaterializedFile *)0)->language_id) == sizeof(((UmiAiCodingContextFile *)0)->language_id), "Owned and selected language IDs share capacity");

/* Inspect only metadata here: an accepted plan is not renewed privacy consent
 * or proof that an adapter's filesystem root contains a requested path. */
static UmiStatus materialization_plan_validate(const UmiAiCodingTaskPlan *plan)
{
    uint32_t tokens = 0U;
    if (plan == NULL || !plan->ready ||
        plan->repository_context.file_count > UMI_AI_CODING_CONTEXT_PLAN_MAX ||
        memchr(plan->request.active_path, '\0', sizeof plan->request.active_path) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->request.active_path[0] != '\0' &&
        !umi_ai_coding_path_is_safe_relative(plan->request.active_path))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < plan->repository_context.file_count; ++i) {
        const UmiAiCodingContextFile *file = &plan->repository_context.files[i];
        if (UmiAiCodingContextFileValidate(file) != UMI_STATUS_OK)
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(file->path, plan->repository_context.files[j].path) == 0)
                return UMI_STATUS_INVALID_ARGUMENT;
        if (file->estimated_tokens > UINT32_MAX - tokens)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        tokens += file->estimated_tokens;
    }
    return UMI_STATUS_OK;
}

/* Share selected metadata admission with prompt consumers without duplicating
 * path, token and count rules in individual applications. */
UmiStatus UmiAiCodingMaterializationPlanValidate(const UmiAiCodingTaskPlan *plan)
{
    return materialization_plan_validate(plan);
}

UmiStatus UmiAiCodingMaterializedContextValidate(const UmiAiCodingMaterializedContext *context)
{
    static const UmiSnapshotTextField fields[] = {
        {"path", offsetof(UmiAiCodingMaterializedFile, path), sizeof(((UmiAiCodingMaterializedFile *)0)->path), 1},
        {"language_id", offsetof(UmiAiCodingMaterializedFile, language_id), sizeof(((UmiAiCodingMaterializedFile *)0)->language_id), 0}
    };
    uint32_t tokens = 0U;
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (context->file_count > UMI_AI_CODING_RUNTIME_CONTEXT_CAPACITY ||
        context->skipped_count > UMI_AI_CODING_CONTEXT_PLAN_MAX - context->file_count)
        return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < context->file_count; ++i) {
        const UmiAiCodingMaterializedFile *file = &context->files[i];
        if (UmiSnapshotValidateTextFields(file, sizeof *file, fields,
                sizeof fields / sizeof fields[0], NULL) != UMI_STATUS_OK ||
            !umi_ai_coding_path_is_safe_relative(file->path) ||
            file->length >= sizeof file->text ||
            memchr(file->text, '\0', file->length) != NULL ||
            file->text[file->length] != '\0' || file->estimated_tokens == 0U ||
            file->estimated_tokens > UINT32_MAX - tokens)
            return UMI_STATUS_INVALID_STATE;
        /* The non-cryptographic hash detects accidental content mismatch; it
         * is not a signature or a source of execution authority. */
        if (file->hash != umi_ai_coding_text_hash(file->text, file->length))
            return UMI_STATUS_INVALID_STATE;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(file->path, context->files[j].path) == 0)
                return UMI_STATUS_INVALID_STATE;
        tokens += file->estimated_tokens;
    }
    return tokens == context->estimated_tokens ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

/* Hold the input snapshot and result away from the Windows thread stack.
 * Callbacks may edit the original metadata/adapter, but this operation keeps
 * its admitted selection and function pointers until it returns. User data
 * remains borrowed and must stay alive; this is not thread synchronization. */
typedef struct MaterializationWork {
    UmiAiCodingTaskPlan plan;
    UmiAiCodingMaterializedContext result;
} MaterializationWork;

UmiStatus umi_ai_coding_materialize_context(
    const UmiAiCodingTaskPlan *plan,
    const UmiAiCodingWorkspaceAdapter *workspace,
    UmiAiCodingMaterializedContext *out_context)
{
    UmiAiCodingWorkspaceAdapter adapter;
    MaterializationWork *work;
    UmiStatus status;
    if (out_context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = materialization_plan_validate(plan);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_coding_workspace_adapter_validate(workspace);
    if (status != UMI_STATUS_OK) return status;
    adapter = *workspace;
    work = calloc(1U, sizeof *work);
    if (work == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    work->plan = *plan;
    work->result.revision = work->plan.revision;
    for (size_t i = 0U; i < work->plan.repository_context.file_count; ++i) {
        const UmiAiCodingContextFile *source = &work->plan.repository_context.files[i];
        UmiAiCodingMaterializedFile *target;
        size_t length = 0U;
        if (work->result.file_count >= UMI_AI_CODING_RUNTIME_CONTEXT_CAPACITY) {
            work->result.truncated = 1;
            work->result.skipped_count += work->plan.repository_context.file_count - i;
            break;
        }
        target = &work->result.files[work->result.file_count];
        memset(target, 0, sizeof *target);
        status = adapter.read(adapter.user_data, source->path, target->text, sizeof target->text, &length);
        if (status == UMI_STATUS_NOT_FOUND || status == UMI_STATUS_CAPACITY_EXCEEDED ||
            status == UMI_STATUS_PARSE_ERROR) {
            ++work->result.skipped_count;
            if (status == UMI_STATUS_CAPACITY_EXCEEDED) work->result.truncated = 1;
            /* Failed callback bytes are not part of the published context. */
            memset(target, 0, sizeof *target);
            status = UMI_STATUS_OK;
            continue;
        }
        if (status != UMI_STATUS_OK) break;
        /* A successful callback must report bytes within the supplied buffer.
         * Check before hashing, indexing or publishing a text terminator. */
        if (length >= sizeof target->text) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        if (memchr(target->text, '\0', length) != NULL) {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
        target->text[length] = '\0';
        memcpy(target->path, source->path, sizeof target->path);
        memcpy(target->language_id, source->language_id, sizeof target->language_id);
        target->length = length;
        target->hash = umi_ai_coding_text_hash(target->text, length);
        target->estimated_tokens = source->estimated_tokens;
        target->active = work->plan.request.active_path[0] != '\0' &&
            strcmp(source->path, work->plan.request.active_path) == 0;
        work->result.estimated_tokens += source->estimated_tokens;
        ++work->result.file_count;
    }
    if (status == UMI_STATUS_OK) status = UmiAiCodingMaterializedContextValidate(&work->result);
    if (status == UMI_STATUS_OK) memcpy(out_context, &work->result, sizeof *out_context);
    free(work);
    return status;
}

UmiStatus umi_ai_coding_materialized_context_find(
    const UmiAiCodingMaterializedContext *context,
    const char *path,
    UmiAiCodingMaterializedFile *out_file)
{
    UmiStatus status;
    size_t length = 0U;
    if (path == NULL || out_file == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    while (length < UMI_AI_CODING_RUNTIME_PATH_CAPACITY && path[length] != '\0') ++length;
    if (length == UMI_AI_CODING_RUNTIME_PATH_CAPACITY ||
        !umi_ai_coding_path_is_safe_relative(path)) return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiCodingMaterializedContextValidate(context);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < context->file_count; ++i) {
        if (strcmp(context->files[i].path, path) == 0) {
            /* A query may borrow an existing record; publish after all reads. */
            memmove(out_file, &context->files[i], sizeof *out_file);
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
