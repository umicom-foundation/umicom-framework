/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/generator.c
 *
 * PURPOSE:
 *   Apply project-generation plans through the canonical Framework filesystem.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/generator.h"

#include <string.h>

#include "umicom/platform/filesystem.h"

/*
 * Provide the ensure parent directory operation used by this module and its client
 * applications.
 */
static UmiStatus ensure_parent_directory(
    const char *file_path,
    UmiDeveloperProjectGeneratorReport *report,
    int dry_run)
{
    char parent[UMI_DEVELOPER_PROJECT_PATH_CAPACITY];
    UmiStatus status;

    status = umi_fs_parent(parent, sizeof(parent), file_path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_fs_is_directory(parent)) return UMI_STATUS_OK;

    /* Apply this branch only when its contract condition is satisfied. */
    if (!dry_run) {
        status = umi_fs_make_directories(parent);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report != NULL) report->directories_created += 1U;
    return UMI_STATUS_OK;
}

/*
 * Perform developer project generator through the module contract so client applications
 * do not duplicate its policy.
 */
UmiStatus umi_developer_project_generator_apply(
    const UmiDeveloperProjectGeneratorRequest *request,
    UmiDeveloperProjectGeneratorReport *out_report)
{
    UmiDeveloperProjectGeneratorReport local_report;
    UmiDeveloperProjectGeneratorReport *report =
        out_report != NULL ? out_report : &local_report;
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL || request->plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_project_generation_plan_validate(request->plan);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)memset(report, 0, sizeof(*report));
    report->dry_run = request->dry_run != 0;

    /* Apply this branch only when its contract condition is satisfied. */
    if (!request->dry_run &&
        !umi_fs_is_directory(request->plan->project_root)) {
        status = umi_fs_make_directories(request->plan->project_root);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        report->directories_created += 1U;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < request->plan->file_count; ++index) {
        const UmiDeveloperProjectGeneratedFile *file =
            &request->plan->files[index];
        char destination[UMI_DEVELOPER_PROJECT_PATH_CAPACITY];

        status = umi_fs_join(
            destination,
            sizeof(destination),
            request->plan->project_root,
            file->relative_path);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_fs_exists(destination) &&
            !request->overwrite_existing_files) {
            report->files_skipped += 1U;
            continue;
        }

        status = ensure_parent_directory(
            destination,
            report,
            request->dry_run);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        /* Apply this branch only when its contract condition is satisfied. */
        if (!request->dry_run) {
            status = umi_fs_write_text(destination, file->content);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
        }

        report->files_created += 1U;
        report->bytes_written += strlen(file->content);
    }

    return UMI_STATUS_OK;
}
