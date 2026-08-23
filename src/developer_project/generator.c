/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/generator.c
 *
 * PURPOSE:
 *   Apply project-generation plans through the canonical Framework filesystem.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/generator.h"

#include <string.h>

#include "umicom/platform/filesystem.h"

static UmiStatus ensure_parent_directory(
    const char *file_path,
    UmiDeveloperProjectGeneratorReport *report,
    int dry_run)
{
    char parent[UMI_DEVELOPER_PROJECT_PATH_CAPACITY];
    UmiStatus status;

    status = umi_fs_parent(parent, sizeof(parent), file_path);
    if (status != UMI_STATUS_OK) return status;

    if (umi_fs_is_directory(parent)) return UMI_STATUS_OK;

    if (!dry_run) {
        status = umi_fs_make_directories(parent);
        if (status != UMI_STATUS_OK) return status;
    }

    if (report != NULL) report->directories_created += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_project_generator_apply(
    const UmiDeveloperProjectGeneratorRequest *request,
    UmiDeveloperProjectGeneratorReport *out_report)
{
    UmiDeveloperProjectGeneratorReport local_report;
    UmiDeveloperProjectGeneratorReport *report =
        out_report != NULL ? out_report : &local_report;
    size_t index;
    UmiStatus status;

    if (request == NULL || request->plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_project_generation_plan_validate(request->plan);
    if (status != UMI_STATUS_OK) return status;

    (void)memset(report, 0, sizeof(*report));
    report->dry_run = request->dry_run != 0;

    if (!request->dry_run &&
        !umi_fs_is_directory(request->plan->project_root)) {
        status = umi_fs_make_directories(request->plan->project_root);
        if (status != UMI_STATUS_OK) return status;
        report->directories_created += 1U;
    }

    for (index = 0U; index < request->plan->file_count; ++index) {
        const UmiDeveloperProjectGeneratedFile *file =
            &request->plan->files[index];
        char destination[UMI_DEVELOPER_PROJECT_PATH_CAPACITY];

        status = umi_fs_join(
            destination,
            sizeof(destination),
            request->plan->project_root,
            file->relative_path);
        if (status != UMI_STATUS_OK) return status;

        if (umi_fs_exists(destination) &&
            !request->overwrite_existing_files) {
            report->files_skipped += 1U;
            continue;
        }

        status = ensure_parent_directory(
            destination,
            report,
            request->dry_run);
        if (status != UMI_STATUS_OK) return status;

        if (!request->dry_run) {
            status = umi_fs_write_text(destination, file->content);
            if (status != UMI_STATUS_OK) return status;
        }

        report->files_created += 1U;
        report->bytes_written += strlen(file->content);
    }

    return UMI_STATUS_OK;
}
