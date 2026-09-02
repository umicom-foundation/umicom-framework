/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/change_guard.c
 *
 * PURPOSE:
 *   Implement conservative pre-approval patch safety checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/change_guard.h"

#include "umicom/base/text.h"

#include <stdio.h>
#include <string.h>

/*
 * Provide the contains component operation used by this module and its client
 * applications.
 */
static int contains_component(const char *path, const char *component)
{
    const char *cursor = path;
    const size_t component_length = strlen(component);

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (cursor != NULL && *cursor != '\0') {
        const char *end = strchr(cursor, '/');
        const size_t length =
            end != NULL ? (size_t)(end - cursor) : strlen(cursor);

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length == component_length &&
            strncmp(cursor, component, length) == 0) {
            return 1;
        }

        cursor = end != NULL ? end + 1 : NULL;
    }

    return 0;
}

/*
 * Initialise ai coding change guard policy from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_coding_change_guard_policy_init(
    UmiAiCodingChangeGuardPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) return;

    policy->maximum_files = 12U;
    policy->maximum_changed_lines = 2000U;
    policy->allow_delete = 0;
    policy->allow_generated_paths = 0;
    policy->allow_dependency_paths = 0;
}

/*
 * Provide the ai coding change guard check operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_change_guard_check(
    const UmiAiCodingPatch *patch,
    const UmiAiCodingChangeGuardPolicy *policy,
    UmiAiCodingChangeGuardResult *out_result)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (patch == NULL || policy == NULL || out_result == NULL ||
        policy->maximum_files == 0U ||
        policy->maximum_changed_lines == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_result, 0, sizeof(*out_result));
    out_result->accepted = 1;

    /* Apply this branch only when its contract condition is satisfied. */
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

    /* Apply this branch only when its contract condition is satisfied. */
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

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < patch->file_count; ++index) {
        const UmiAiCodingPatchFile *file = &patch->files[index];

        /* Apply this branch only when its contract condition is satisfied. */
        if (!policy->allow_delete &&
            file->operation == UMI_AI_CODING_PATCH_DELETE) {
            out_result->accepted = 0;
            out_result->rejected_files += 1U;
            (void)umi_text_format(
                out_result->reason,
                sizeof(out_result->reason),
                "Deletion is disabled by coding-agent policy: %s",
                file->path);
            return UMI_STATUS_OK;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (!policy->allow_generated_paths &&
            (contains_component(file->path, "build") ||
             contains_component(file->path, "dist") ||
             contains_component(file->path, "install") ||
             contains_component(file->path, "out"))) {
            out_result->accepted = 0;
            out_result->rejected_files += 1U;
            (void)umi_text_format(
                out_result->reason,
                sizeof(out_result->reason),
                "Generated/build output path is protected: %s",
                file->path);
            return UMI_STATUS_OK;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (!policy->allow_dependency_paths &&
            (contains_component(file->path, "node_modules") ||
             contains_component(file->path, "vendor") ||
             contains_component(file->path, ".venv"))) {
            out_result->accepted = 0;
            out_result->rejected_files += 1U;
            (void)umi_text_format(
                out_result->reason,
                sizeof(out_result->reason),
                "Dependency path is protected: %s",
                file->path);
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_OK;
}
