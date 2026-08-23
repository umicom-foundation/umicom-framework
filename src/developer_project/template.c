/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/template.c
 *
 * PURPOSE:
 *   Validate reusable project-template contracts before generation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/template.h"

#include <string.h>

UmiStatus umi_developer_project_template_validate(
    const UmiDeveloperProjectTemplate *project_template)
{
    size_t index;
    size_t other;

    if (project_template == NULL ||
        project_template->structure_size != sizeof(*project_template) ||
        project_template->api_version != UMI_DEVELOPER_PROJECT_API_VERSION ||
        project_template->template_id == NULL ||
        project_template->template_id[0] == '\0' ||
        project_template->title == NULL ||
        project_template->title[0] == '\0' ||
        project_template->description == NULL ||
        project_template->kind < UMI_DEVELOPER_PROJECT_EXECUTABLE ||
        project_template->kind > UMI_DEVELOPER_PROJECT_META ||
        project_template->build_system < UMI_DEVELOPER_PROJECT_BUILD_NONE ||
        project_template->build_system > UMI_DEVELOPER_PROJECT_BUILD_CUSTOM ||
        project_template->primary_language_id == NULL ||
        project_template->primary_language_id[0] == '\0' ||
        project_template->secondary_language_count >
            UMI_DEVELOPER_PROJECT_LANGUAGE_CAPACITY ||
        (project_template->secondary_language_count > 0U &&
         project_template->secondary_language_ids == NULL) ||
        project_template->files == NULL ||
        project_template->file_count == 0U ||
        project_template->file_count >
            UMI_DEVELOPER_PROJECT_TEMPLATE_FILE_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < project_template->file_count; ++index) {
        const UmiDeveloperProjectTemplateFile *file =
            &project_template->files[index];

        if (file->relative_path == NULL ||
            file->relative_path[0] == '\0' ||
            file->content_template == NULL ||
            file->relative_path[0] == '/' ||
            file->relative_path[0] == '\\') {
            return UMI_STATUS_INVALID_ARGUMENT;
        }

        if (strstr(file->relative_path, "..") != NULL) {
            return UMI_STATUS_PERMISSION_DENIED;
        }

        for (other = index + 1U;
             other < project_template->file_count;
             ++other) {
            if (strcmp(file->relative_path,
                       project_template->files[other].relative_path) == 0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
    }

    return UMI_STATUS_OK;
}
