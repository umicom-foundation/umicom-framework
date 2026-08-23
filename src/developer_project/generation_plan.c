/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/generation_plan.c
 *
 * PURPOSE:
 *   Render file paths/content from a project template into a reviewable plan.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/generation_plan.h"

#include <string.h>

static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(source);
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

void umi_developer_project_generation_plan_init(
    UmiDeveloperProjectGenerationPlan *plan)
{
    if (plan == NULL) return;

    (void)memset(plan, 0, sizeof(*plan));
    plan->revision = 1U;
}

UmiStatus umi_developer_project_generation_plan_build(
    const UmiDeveloperProjectTemplate *project_template,
    const UmiDeveloperProjectVariableSet *variables,
    const char *project_root,
    UmiDeveloperProjectGenerationPlan *out_plan)
{
    size_t index;
    UmiStatus status;

    if (project_template == NULL || variables == NULL ||
        project_root == NULL || project_root[0] == '\0' ||
        out_plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_project_template_validate(project_template);
    if (status != UMI_STATUS_OK) return status;

    umi_developer_project_generation_plan_init(out_plan);

    status = copy_text(
        out_plan->project_root,
        sizeof(out_plan->project_root),
        project_root);
    if (status != UMI_STATUS_OK) return status;

    status = copy_text(
        out_plan->template_id,
        sizeof(out_plan->template_id),
        project_template->template_id);
    if (status != UMI_STATUS_OK) return status;

    if (project_template->file_count >
        UMI_DEVELOPER_PROJECT_GENERATION_FILE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    for (index = 0U; index < project_template->file_count; ++index) {
        const UmiDeveloperProjectTemplateFile *source =
            &project_template->files[index];
        UmiDeveloperProjectGeneratedFile *destination =
            &out_plan->files[out_plan->file_count];

        status = umi_developer_project_render_text(
            source->relative_path,
            variables,
            destination->relative_path,
            sizeof(destination->relative_path));
        if (status != UMI_STATUS_OK) return status;

        status = umi_developer_project_render_text(
            source->content_template,
            variables,
            destination->content,
            sizeof(destination->content));
        if (status != UMI_STATUS_OK) return status;

        destination->executable = source->executable;
        destination->optional = source->optional;
        out_plan->total_content_bytes += strlen(destination->content);
        out_plan->file_count += 1U;
    }

    out_plan->revision += 1U;
    return umi_developer_project_generation_plan_validate(out_plan);
}

UmiStatus umi_developer_project_generation_plan_validate(
    const UmiDeveloperProjectGenerationPlan *plan)
{
    size_t index;
    size_t other;

    if (plan == NULL ||
        plan->project_root[0] == '\0' ||
        plan->template_id[0] == '\0' ||
        plan->file_count == 0U ||
        plan->file_count > UMI_DEVELOPER_PROJECT_GENERATION_FILE_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < plan->file_count; ++index) {
        const char *path = plan->files[index].relative_path;

        if (path[0] == '\0' ||
            path[0] == '/' || path[0] == '\\' ||
            strstr(path, "..") != NULL) {
            return UMI_STATUS_PERMISSION_DENIED;
        }

        for (other = index + 1U; other < plan->file_count; ++other) {
            if (strcmp(path, plan->files[other].relative_path) == 0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
    }

    return UMI_STATUS_OK;
}
