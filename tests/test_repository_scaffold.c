/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_repository_scaffold.c
 *
 * PURPOSE:
 *   Generate a complete local Umicom Designer repository from the production
 *   repository template, verify its key files and token replacement, and remove
 *   the temporary fixture without requiring a remote GitHub repository.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef UMICOM_REPOSITORY_TEMPLATE_ROOT
#define UMICOM_REPOSITORY_TEMPLATE_ROOT "templates/repository"
#endif

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiToolchainProfile profile;
    UmiEnvironmentPlan environment;
    UmiRepositoryScaffoldRequest request;
    UmiRepositoryScaffoldReport report;
    UmiToolInfo *git_tool;
    char temporary[UMI_PATH_CAPACITY];
    char parent[UMI_PATH_CAPACITY];
    char cmake_path[UMI_PATH_CAPACITY];
    char application_path[UMI_PATH_CAPACITY];
    char *application_text = NULL;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_fs_temp_directory(temporary, sizeof(temporary)) != UMI_STATUS_OK ||
        umi_fs_join(parent,
                    sizeof(parent),
                    temporary,
                    "umicom-scaffold-test-parent") != UMI_STATUS_OK)
        return EXIT_FAILURE;
    (void)umi_fs_remove_tree(parent);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_fs_make_directories(parent) != UMI_STATUS_OK)
        return EXIT_FAILURE;

    umi_toolchain_profile_init(&profile);
    git_tool = umi_toolchain_profile_tool_mutable(&profile, UMI_TOOL_GIT);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (git_tool == NULL) return EXIT_FAILURE;
    git_tool->state = UMI_TOOL_VALIDATED;
    (void)snprintf(git_tool->path, sizeof(git_tool->path), "%s", "git");
    umi_environment_plan_init(&environment);
    (void)memset(&request, 0, sizeof(request));
    request.template_root = UMICOM_REPOSITORY_TEMPLATE_ROOT;
    request.destination_parent = parent;
    request.application_name = "Umicom Designer";
    request.repository_name = "umicom-designer";
    request.application_id = "org.umicom.designer";
    request.frontends = UMI_FRONTEND_CONSOLE | UMI_FRONTEND_GTK4;
    request.framework_link = UMI_FRAMEWORK_LINK_INSTALLED;
    request.initialise_git = 0;
    request.create_initial_commit = 0;

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_repository_scaffold_create(&profile,
                                       &environment,
                                       &request,
                                       &report) != UMI_STATUS_OK)
        return EXIT_FAILURE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (report.files_created < 10U || report.directories_created < 5U)
        return EXIT_FAILURE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_fs_join(cmake_path,
                    sizeof(cmake_path),
                    report.repository_root,
                    "CMakeLists.txt") != UMI_STATUS_OK ||
        !umi_fs_is_file(cmake_path))
        return EXIT_FAILURE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_fs_join(application_path,
                    sizeof(application_path),
                    report.repository_root,
                    "applications/umicom-designer/src/app/application.c") !=
        UMI_STATUS_OK)
        return EXIT_FAILURE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_fs_read_text(application_path,
                         &application_text,
                         NULL) != UMI_STATUS_OK)
        return EXIT_FAILURE;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strstr(application_text, "@APP_NAME@") != NULL ||
        strstr(application_text, "Umicom Designer") == NULL) {
        umi_fs_free_text(application_text);
        return EXIT_FAILURE;
    }
    umi_fs_free_text(application_text);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_fs_remove_tree(parent) != UMI_STATUS_OK)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
