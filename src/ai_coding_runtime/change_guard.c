/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/change_guard.c
 *
 * PURPOSE:
 *   Implement conservative pre-approval patch safety checks.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/change_guard.h"

#include <stdio.h>
#include <string.h>

static int contains_component(const char *path, const char *component)
{
    const char *cursor = path;
    const size_t component_length = strlen(component);

    while (cursor != NULL && *cursor != '\0') {
        const char *end = strchr(cursor, '/');
        const size_t length =
            end != NULL ? (size_t)(end - cursor) : strlen(cursor);

        if (length == component_length &&
            strncmp(cursor, component, length) == 0) {
            return 1;
        }

        cursor = end != NULL ? end + 1 : NULL;
    }

    return 0;
}

void umi_ai_coding_change_guard_policy_init(
    UmiAiCodingChangeGuardPolicy *policy)
{
    if (policy == NULL) return;

    policy->maximum_files = 12U;
    policy->maximum_changed_lines = 2000U;
    policy->allow_delete = 0;
    policy->allow_generated_paths = 0;
    policy->allow_dependency_paths = 0;
}

UmiStatus umi_ai_coding_change_guard_check(
    const UmiAiCodingPatch *patch,
    const UmiAiCodingChangeGuardPolicy *policy,
    UmiAiCodingChangeGuardResult *out_result)
{
    size_t index;

    if (patch == NULL || policy == NULL || out_result == NULL ||
        policy->maximum_files == 0U ||
        policy->maximum_changed_lines == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_result, 0, sizeof(*out_result));
    out_result->accepted = 1;

    if (patch->file_count > policy->maximum_files) {
        out_result->accepted = 0;
        (void)snprintf(
            out_result->reason,
            sizeof(out_result->reason),
            "Patch contains %zu files; policy permits at most %zu.",
            patch->file_count,
            policy->maximum_files);
        return UMI_STATUS_OK;
    }

    if (patch->changed_lines > policy->maximum_changed_lines) {
        out_result->accepted = 0;
        (void)snprintf(
            out_result->reason,
            sizeof(out_result->reason),
            "Patch changes %u lines; policy permits at most %u.",
            patch->changed_lines,
            policy->maximum_changed_lines);
        return UMI_STATUS_OK;
    }

    for (index = 0U; index < patch->file_count; ++index) {
        const UmiAiCodingPatchFile *file = &patch->files[index];

        if (!policy->allow_delete &&
            file->operation == UMI_AI_CODING_PATCH_DELETE) {
            out_result->accepted = 0;
            out_result->rejected_files += 1U;
            (void)snprintf(
                out_result->reason,
                sizeof(out_result->reason),
                "Deletion is disabled by coding-agent policy: %s",
                file->path);
            return UMI_STATUS_OK;
        }

        if (!policy->allow_generated_paths &&
            (contains_component(file->path, "build") ||
             contains_component(file->path, "dist") ||
             contains_component(file->path, "install") ||
             contains_component(file->path, "out"))) {
            out_result->accepted = 0;
            out_result->rejected_files += 1U;
            (void)snprintf(
                out_result->reason,
                sizeof(out_result->reason),
                "Generated/build output path is protected: %s",
                file->path);
            return UMI_STATUS_OK;
        }

        if (!policy->allow_dependency_paths &&
            (contains_component(file->path, "node_modules") ||
             contains_component(file->path, "vendor") ||
             contains_component(file->path, ".venv"))) {
            out_result->accepted = 0;
            out_result->rejected_files += 1U;
            (void)snprintf(
                out_result->reason,
                sizeof(out_result->reason),
                "Dependency path is protected: %s",
                file->path);
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_OK;
}
