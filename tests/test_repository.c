/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_repository.c
 *
 * PURPOSE:
 *   Verify native Git repository planning through the safe dry-run path without
 *   creating a remote repository or changing the developer's Git configuration.
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

int main(void)
{
    UmiToolchainProfile profile;
    UmiEnvironmentPlan environment;
    UmiRepositoryRequest request;
    UmiRepositoryReport report;
    UmiToolInfo *git_tool;

    umi_toolchain_profile_init(&profile);
    git_tool = umi_toolchain_profile_tool_mutable(&profile, UMI_TOOL_GIT);
    if (git_tool == NULL) return EXIT_FAILURE;
    git_tool->state = UMI_TOOL_VALIDATED;
    (void)snprintf(git_tool->path, sizeof(git_tool->path), "%s", "git");
    umi_environment_plan_init(&environment);
    (void)memset(&request, 0, sizeof(request));
    request.repository_root = ".";
    request.remote_name = "umicom-test";
    request.initialise_local = 1;
    request.create_initial_commit = 1;
    request.dry_run = 1;
    if (umi_repository_initialise(&profile,
                                  &environment,
                                  &request,
                                  &report) != UMI_STATUS_OK)
        return EXIT_FAILURE;
    if (!report.local_initialised || !report.initial_commit_created)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
