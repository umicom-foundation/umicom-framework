/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/new_project.c
 *
 * PURPOSE:
 *   Connect safe New Project input to the shared generator and build profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/new_project.h"
#include "umicom/platform/filesystem.h"

/* A new project is different from applying an additive recipe to existing files. */
UmiStatus UmiDeveloperProjectCreateNew(UmiDeveloperProjectService *service,
    const UmiDeveloperProjectGenerationRequest *request,
    UmiDeveloperProjectGeneratorReport *outReport,
    UmiDeveloperProjectModel *outModel, UmiBuildProfile *outProfile)
{
    UmiDeveloperProjectModel model;
    UmiDeveloperProjectGeneratorReport report = {0};
    UmiBuildProfile profile;
    UmiStatus status;
    if (service == NULL || request == NULL || outModel == NULL || outProfile == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_developer_project_generation_request_validate(request, NULL, 0U);
    if (status != UMI_STATUS_OK) return status;
    if (!umi_fs_is_absolute(request->project_root) || request->overwrite_existing_files)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_fs_exists(request->project_root)) return UMI_STATUS_ALREADY_EXISTS;
    status = umi_developer_project_service_generate(service, request, &report, &model);
    if (outReport != NULL) *outReport = report;
    if (status != UMI_STATUS_OK) return status;
    if (report.files_skipped != 0U) return UMI_STATUS_ALREADY_EXISTS;
    status = UmiBuildProfileFromProject(&model, &profile);
    if (status != UMI_STATUS_OK) return status;
    *outModel = model;
    *outProfile = profile;
    return UMI_STATUS_OK;
}
