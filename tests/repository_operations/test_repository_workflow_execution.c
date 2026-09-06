/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_operations/test_repository_workflow_execution.c
 *
 * PURPOSE:
 *   Exercise real process exit handling using this executable as a fake Git.
 *   No Git repository, credentials, commit, network, or remote is touched.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/workflow_executor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The child process recognizes only the argument vectors used by the workflow.
 * Distinct failures verify that the expected diff exit does not hide errors
 * from validation or commit hooks. */
static int fake_git(int argc, char **argv, const char *scenario)
{
    if (argc == 3 && strcmp(argv[1], "add") == 0 &&
        strcmp(argv[2], "-A") == 0) return 0;
    if (argc == 4 && strcmp(argv[1], "status") == 0) {
        (void)puts("## main...origin/main");
        return 0;
    }
    if (argc >= 3 && strcmp(argv[1], "diff") == 0) {
        if (strcmp(argv[argc - 1], "--quiet") == 0) {
            if (strcmp(scenario, "clean") == 0) return 0;
            if (strcmp(scenario, "diff-error") == 0) {
                (void)fputs("simulated diff failure\n", stderr);
                return 2;
            }
            return 1;
        }
        if (strcmp(argv[argc - 1], "--check") == 0) {
            if (strcmp(scenario, "check-error") == 0) {
                (void)fputs("simulated whitespace violation\n", stderr);
                return 2;
            }
            return 0;
        }
        if (strcmp(argv[argc - 1], "--name-only") == 0) {
            (void)puts("src/gtk/main.c\nsrc/gtk/workstation.c");
            return 0;
        }
    }
    if (argc == 4 && strcmp(argv[1], "commit") == 0 &&
        strcmp(argv[2], "-m") == 0 && argv[3][0] != '\0') {
        if (strcmp(scenario, "commit-error") == 0) {
            (void)fputs("simulated commit hook failure\n", stderr);
            return 1;
        }
        (void)puts("fake commit completed");
        return 0;
    }
    if (argc == 4 && strcmp(argv[1], "push") == 0) {
        (void)puts("fake push completed");
        return 0;
    }
    (void)fputs("unexpected fake Git command\n", stderr);
    return 99;
}

/* Run a complete workflow and verify its result, diagnostics, and final steps. */
static int verify_scenario(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    UmiRepositoryWorkflowReport *report,
    const char *scenario,
    UmiRepositoryWorkflowAction action,
    UmiStatus expected_status,
    int expected_exit,
    int expected_commit,
    int expected_push,
    const char *expected_text)
{
    UmiRepositoryWorkflowRequest request;
    UmiStatus status;

    umi_environment_plan_init(environment);
    if (umi_environment_plan_add(environment, "UMICOM_TEST_FAKE_GIT", scenario)
        != UMI_STATUS_OK) return 0;
    umi_repository_workflow_request_init(&request, action, ".");
    if (action == UMI_REPOSITORY_WORKFLOW_PUBLISH) {
        request.auto_commit_message = 1;
    }
    status = umi_repository_workflow_execute(
        profile, environment, &request, report);
    if (status != expected_status || report->last_exit_code != expected_exit ||
        report->commit_created != expected_commit || report->pushed != expected_push ||
        strstr(report->output, expected_text) == NULL) {
        (void)fprintf(stderr, "Workflow scenario failed: %s\n%s\n",
                      scenario, report->output);
        return 0;
    }
    if (!expected_commit && strstr(report->output, "fake commit completed") != NULL)
        return 0;
    if (!expected_push && strstr(report->output, "fake push completed") != NULL)
        return 0;
    if (strcmp(scenario, "staged") == 0 &&
        strstr(report->output, "Generated commit message:") == NULL) return 0;
    if (strcmp(scenario, "clean") == 0 && !report->no_changes) return 0;
    return 1;
}

/* Act as the fake child when requested, otherwise verify all workflow cases. */
int main(int argc, char **argv)
{
    const char *scenario = getenv("UMICOM_TEST_FAKE_GIT");
    UmiToolchainProfile *profile;
    UmiEnvironmentPlan *environment;
    UmiRepositoryWorkflowReport *report;
    UmiToolInfo *git;
    int passed;
    int written;

    if (scenario != NULL) return fake_git(argc, argv, scenario);
    profile = (UmiToolchainProfile *)calloc(1U, sizeof(*profile));
    environment = (UmiEnvironmentPlan *)calloc(1U, sizeof(*environment));
    report = (UmiRepositoryWorkflowReport *)calloc(1U, sizeof(*report));
    if (profile == NULL || environment == NULL || report == NULL) {
        free(profile); free(environment); free(report);
        return EXIT_FAILURE;
    }
    umi_toolchain_profile_init(profile);
    git = umi_toolchain_profile_tool_mutable(profile, UMI_TOOL_GIT);
    if (git == NULL) {
        free(profile); free(environment); free(report);
        return EXIT_FAILURE;
    }
    git->state = UMI_TOOL_VALIDATED;
    written = snprintf(git->path, sizeof(git->path), "%s", argv[0]);
    if (written < 0 || (size_t)written >= sizeof(git->path)) {
        free(profile); free(environment); free(report);
        return EXIT_FAILURE;
    }

    passed = verify_scenario(profile, environment, report, "staged",
        UMI_REPOSITORY_WORKFLOW_PUBLISH, UMI_STATUS_OK, 0, 1, 1,
        "fake push completed");
    passed = verify_scenario(profile, environment, report, "clean",
        UMI_REPOSITORY_WORKFLOW_PUBLISH, UMI_STATUS_OK, 0, 0, 1,
        "fake push completed") && passed;
    passed = verify_scenario(profile, environment, report, "diff-error",
        UMI_REPOSITORY_WORKFLOW_PUBLISH, UMI_STATUS_INTERNAL_ERROR, 2, 0, 0,
        "Git command failed: diff") && passed;
    passed = verify_scenario(profile, environment, report, "check-error",
        UMI_REPOSITORY_WORKFLOW_PUBLISH, UMI_STATUS_INTERNAL_ERROR, 2, 0, 0,
        "simulated whitespace violation") && passed;
    passed = verify_scenario(profile, environment, report, "commit-error",
        UMI_REPOSITORY_WORKFLOW_PUBLISH, UMI_STATUS_INTERNAL_ERROR, 1, 0, 0,
        "Git command failed: commit") && passed;
    passed = verify_scenario(profile, environment, report, "update-dirty",
        UMI_REPOSITORY_WORKFLOW_UPDATE, UMI_STATUS_INVALID_STATE, 1, 0, 0,
        "Update stopped: tracked working-tree changes") && passed;

    free(profile); free(environment); free(report);
    return passed ? EXIT_SUCCESS : EXIT_FAILURE;
}
