/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education_workspace/project_workflow.c
 * PURPOSE: Keep lesson build metadata alongside Framework-owned project export.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/education_workspace/project_workflow.h"
#include "private.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct EducationProjectDefinition
{
    const char *id, *title, *executable;
    int sdk;
} EducationProjectDefinition;
/* These executable names match the canonical teaching CMake files. Keep each
 * description beside its source project when another practical course is added. */
static const EducationProjectDefinition PROJECTS[] = {
    {"notes", "Notes practice project", "umicom-notes", 0},
    {"assembly", "Stock totals practice project", "umicom-stock-total", 0},
    {"framework", "Framework storage practice project", "umicom-framework-notes", 1}};
UmiStatus UmiEducationProjectWorkflowDescribe(const char *id, const char *destination,
                                              UmiEducationProjectWorkflow *out)
{
    if (id == NULL || out == NULL || !EwTextValid(destination, 1024U, false))
        return UMI_STATUS_INVALID_ARGUMENT;
    const EducationProjectDefinition *definition = NULL;
    for (size_t index = 0U; index < sizeof PROJECTS / sizeof PROJECTS[0]; ++index)
        if (strcmp(id, PROJECTS[index].id) == 0)
        {
            definition = &PROJECTS[index];
            break;
        }
    if (definition == NULL)
        return UMI_STATUS_NOT_FOUND;
    size_t length = strlen(destination);
    if (!umi_path_is_absolute(destination) || destination[length - 1U] == '/' ||
        destination[length - 1U] == '\\')
        return UMI_STATUS_INVALID_ARGUMENT;
#ifdef _WIN32
    /* Match export's local-drive restriction instead of preparing metadata for
     * a location the writer would refuse after the user selected it. */
    if (length < 3U ||
        !((destination[0] >= 'A' && destination[0] <= 'Z') ||
          (destination[0] >= 'a' && destination[0] <= 'z')) ||
        destination[1] != ':')
        return UMI_STATUS_INVALID_ARGUMENT;
#endif
    UmiEducationProjectWorkflow *candidate = calloc(1U, sizeof *candidate);
    if (candidate == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    umi_developer_project_model_init(&candidate->project, id, definition->title);
    strcpy(candidate->project.root, destination);
    strcpy(candidate->project.entry_point, "main.c");
    strcpy(candidate->project.primary_language_id, "c23");
    strcpy(candidate->project.template_id, id);
    strcpy(candidate->project.build_directory, "build/default");
    strcpy(candidate->project.install_prefix, "build/default/install");
    candidate->project.generated = 1;
    candidate->requires_framework_sdk = definition->sdk;
    UmiStatus status = umi_build_profile_set(&candidate->build, id, destination, "build/default");
    if (status == UMI_STATUS_OK && strcmp(id, "assembly") == 0)
        status = umi_developer_project_model_add_language(&candidate->project, "assembly");
#ifdef _WIN32
    const char *suffix = ".exe";
#else
    const char *suffix = "";
#endif
    int count = snprintf(candidate->build.run_program, sizeof candidate->build.run_program,
                         "build/default/%s%s", definition->executable, suffix);
    if (count < 0 || (size_t)count >= sizeof candidate->build.run_program)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK)
    {
        strcpy(candidate->project.executable, candidate->build.run_program);
        status = umi_developer_project_model_validate(&candidate->project, NULL, 0U);
    }
    if (status == UMI_STATUS_OK)
        status = umi_build_profile_validate(&candidate->build, NULL, 0U);
    if (status == UMI_STATUS_OK)
        *out = *candidate;
    free(candidate);
    return status;
}
UmiStatus UmiEducationProjectExportForDevelopment(const char *id, const char *destination,
                                                  size_t *out_files_written,
                                                  UmiEducationProjectWorkflow *out)
{
    if (out_files_written == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_files_written = 0U;
    /* Validate all model/profile fields before making the first filesystem
     * change. A successful source export must not be followed by a preventable
     * metadata-construction error. Existing files remain the writer's boundary. */
    UmiEducationProjectWorkflow *candidate = malloc(sizeof *candidate);
    if (candidate == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiEducationProjectWorkflowDescribe(id, destination, candidate);
    if (status == UMI_STATUS_OK)
        status = UmiEducationExportProject(id, destination, out_files_written);
    if (status == UMI_STATUS_OK)
        *out = *candidate;
    free(candidate);
    return status;
}
