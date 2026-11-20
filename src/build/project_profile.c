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
#include <stdio.h>
#include <string.h>

/* Build absolute source identity without changing the process working directory. */
/* Absolute project identity no longer consults the process directory. The previous unconditional cwd lookup is retained for comparison; relative callers keep their documented resolution behavior. */
#if 0
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

#endif
UmiStatus UmiBuildProfileForWorkspace(const char *root, UmiBuildProfile *outProfile)
{
    UmiBuildProfile profile;
    char currentDirectory[UMI_BUILD_PATH_CAPACITY];
    UmiStatus status;
    if (root == NULL || root[0] == '\0' || outProfile == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    umi_build_profile_init(&profile);
    /* A user-selected full path already supplies project identity. A deleted,
     * inaccessible or unrelated process directory must not prevent its use. */
    if (umi_path_is_absolute(root))
        status = umi_path_normalise(root, profile.source_directory, sizeof(profile.source_directory));
    else {
        status = umi_fs_current_directory(currentDirectory, sizeof(currentDirectory));
        if (status == UMI_STATUS_OK)
            status = umi_path_absolute(root, currentDirectory, profile.source_directory,
                sizeof(profile.source_directory));
    }
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
    /* Umicom C executes through its bounded VM, not a guessed native binary.
     * Reuse the configured CMake target so Run uses the compiler path selected
     * during configuration and reports output through the existing runner.
     * Other META recipes retain their previous no-native-launch behaviour. */
    if (status == UMI_STATUS_OK && model->kind == UMI_DEVELOPER_PROJECT_META &&
        strcmp(model->primary_language_id, "developer.language.umicom-c") == 0 &&
        strcmp(model->preset, "preview") == 0 &&
        strcmp(model->build_directory, "build/preview") == 0) {
        status = umi_text_copy(profile.run_program, sizeof(profile.run_program), "cmake");
        if (status == UMI_STATUS_OK)
            status = umi_text_copy(profile.run_arguments, sizeof(profile.run_arguments),
                "--build build/preview --target preview");
    }
    /* These generated projects need the runtime chosen by their own CMake
     * configuration. Script-only models still use their existing policy. */
    const int configuredPython = model->kind == UMI_DEVELOPER_PROJECT_SCRIPT &&
        strcmp(model->template_id, "developer.template.python-tool") == 0;
    const int configuredJava = model->kind == UMI_DEVELOPER_PROJECT_META &&
        strcmp(model->template_id, "developer.template.java-console") == 0;
    if (status == UMI_STATUS_OK && (configuredPython || configuredJava) &&
        model->build_system == UMI_DEVELOPER_PROJECT_BUILD_CMAKE) {
        const char *preset = NULL;
        if (strcmp(model->preset, "windows-ucrt64-debug") == 0 &&
            strcmp(model->build_directory, "build/windows-ucrt64-debug") == 0)
            preset = "windows-ucrt64-debug";
        else if (strcmp(model->preset, "linux-debug") == 0 &&
                 strcmp(model->build_directory, "build/linux-debug") == 0)
            preset = "linux-debug";
        if (preset != NULL) {
            status = umi_text_copy(profile.run_program, sizeof profile.run_program, "cmake");
            const int written = snprintf(profile.run_arguments, sizeof profile.run_arguments,
                "--build --preset %s --target run-program", preset);
            if (written < 0 || (size_t)written >= sizeof profile.run_arguments)
                status = UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }
    if (status == UMI_STATUS_OK) status = umi_build_profile_validate(&profile, NULL, 0U);
    if (status == UMI_STATUS_OK) *outProfile = profile;
    return status;
}
