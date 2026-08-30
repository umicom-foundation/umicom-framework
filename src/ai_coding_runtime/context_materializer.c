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
#include <string.h>

UmiStatus umi_ai_coding_materialize_context(
    const UmiAiCodingTaskPlan *plan,
    const UmiAiCodingWorkspaceAdapter *workspace,
    UmiAiCodingMaterializedContext *out_context)
{
    size_t index;
    UmiStatus status;

    if (plan == NULL || workspace == NULL || out_context == NULL ||
        !plan->ready) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_coding_workspace_adapter_validate(workspace);
    if (status != UMI_STATUS_OK) return status;

    (void)memset(out_context, 0, sizeof(*out_context));
    out_context->revision = plan->revision;

    for (index = 0U;
         index < plan->repository_context.file_count;
         ++index) {
        const UmiAiCodingContextFile *source =
            &plan->repository_context.files[index];
        UmiAiCodingMaterializedFile *target;
        size_t length = 0U;

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

        if (status == UMI_STATUS_NOT_FOUND ||
            status == UMI_STATUS_CAPACITY_EXCEEDED ||
            status == UMI_STATUS_PARSE_ERROR) {
            out_context->skipped_count += 1U;
            if (status == UMI_STATUS_CAPACITY_EXCEEDED) {
                out_context->truncated = 1;
            }
            continue;
        }

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

UmiStatus umi_ai_coding_materialized_context_find(
    const UmiAiCodingMaterializedContext *context,
    const char *path,
    UmiAiCodingMaterializedFile *out_file)
{
    size_t index;

    if (context == NULL || path == NULL || out_file == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < context->file_count; ++index) {
        if (strcmp(context->files[index].path, path) == 0) {
            *out_file = context->files[index];
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_NOT_FOUND;
}
