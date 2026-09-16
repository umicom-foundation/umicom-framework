/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/project_profile.c
 *
 * PURPOSE:
 *   Translate workspace/project identity into validated build profiles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/build/project_profile.h"
#include "umicom/base/text.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"

/* Build absolute source identity without changing the process working directory. */
UmiStatus UmiBuildProfileForWorkspace(const char *root, UmiBuildProfile *outProfile)
{
    UmiBuildProfile profile;
    char currentDirectory[UMI_BUILD_PATH_CAPACITY];
    UmiStatus status;
    if (root == NULL || root[0] == '\0' || outProfile == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_fs_current_directory(currentDirectory, sizeof(currentDirectory));
    if (status != UMI_STATUS_OK) return status;
    umi_build_profile_init(&profile);
    status = umi_path_absolute(root, currentDirectory, profile.source_directory,
        sizeof(profile.source_directory));
    if (status != UMI_STATUS_OK) return status;
    status = umi_text_copy(profile.profile_id, sizeof(profile.profile_id), "project.development");
    if (status == UMI_STATUS_OK) status = umi_text_copy(profile.build_directory,
        sizeof(profile.build_directory), "build/umicom-development");
    if (status == UMI_STATUS_OK) status = umi_text_copy(profile.install_directory,
        sizeof(profile.install_directory), "install/umicom-development");
    if (status != UMI_STATUS_OK) return status;
    profile.parallel_jobs = 2U;
    profile.run_program[0] = '\0';
    *outProfile = profile;
    return UMI_STATUS_OK;
}

/* Preserve the generator's selected target and paths; never launch Studio as
 * the default program of a different project. */
UmiStatus UmiBuildProfileFromProject(const UmiDeveloperProjectModel *model,
    UmiBuildProfile *outProfile)
{
    UmiBuildProfile profile;
    UmiStatus status;
    if (model == NULL || outProfile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_developer_project_model_validate(model, NULL, 0U);
    if (status != UMI_STATUS_OK) return status;
    status = UmiBuildProfileForWorkspace(model->root, &profile);
    if (status != UMI_STATUS_OK) return status;
    status = umi_text_copy(profile.preset, sizeof(profile.preset), model->preset);
    if (status == UMI_STATUS_OK && model->build_directory[0] != '\0')
        status = umi_text_copy(profile.build_directory, sizeof(profile.build_directory), model->build_directory);
    if (status == UMI_STATUS_OK && model->install_prefix[0] != '\0')
        status = umi_text_copy(profile.install_directory, sizeof(profile.install_directory), model->install_prefix);
    if (status == UMI_STATUS_OK && model->kind == UMI_DEVELOPER_PROJECT_EXECUTABLE)
        status = umi_text_copy(profile.run_program, sizeof(profile.run_program), model->executable);
    if (status != UMI_STATUS_OK) return status;
    status = umi_build_profile_validate(&profile, NULL, 0U);
    if (status == UMI_STATUS_OK) *outProfile = profile;
    return status;
}
