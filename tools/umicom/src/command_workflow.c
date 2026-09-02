/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_workflow.c
 *
 * PURPOSE:
 *   Compose environment checks, CodeGuard analysis, dependency auditing,
 *   configure/compile/link/test actions and safe repository operations into one
 *   inspectable Umicom development workflow.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "cli.h"

#include "umicom/developer/lifecycle_plan.h"

#include <stdio.h>
#include <string.h>

typedef struct UmiCliWorkflowOptions {
    const char *source;
    const char *preset;
    const char *jobs;
    const char *message;
    const char *remote;
    const char *branch;
    const char *plan_goal;
    int strict;
    int dry_run;
    int no_submodules;
} UmiCliWorkflowOptions;

/*
 * Provide the workflow option value operation used by this module and its client
 * applications.
 */
static const char *workflow_option_value(
    int argc,
    char **argv,
    const char *option)
{
    int index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0; index + 1 < argc; ++index)
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(argv[index], option) == 0) return argv[index + 1];
    return NULL;
}

/* Provide the workflow has flag operation used by this module and its client applications. */
static int workflow_has_flag(int argc, char **argv, const char *option)
{
    int index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0; index < argc; ++index)
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(argv[index], option) == 0) return 1;
    return 0;
}

/*
 * Read workflow options into validated module state and return a status when input cannot
 * be used.
 */
static int workflow_options_parse(
    int argc,
    char **argv,
    UmiCliWorkflowOptions *options)
{
    int index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (options == NULL) return 0;
    (void)memset(options, 0, sizeof(*options));
    options->source = workflow_option_value(argc, argv, "--source");
    options->preset = workflow_option_value(argc, argv, "--preset");
    options->jobs = workflow_option_value(argc, argv, "--jobs");
    options->message = workflow_option_value(argc, argv, "--message");
    options->remote = workflow_option_value(argc, argv, "--remote");
    options->branch = workflow_option_value(argc, argv, "--branch");
    options->plan_goal = workflow_option_value(argc, argv, "--goal");
    options->strict = workflow_has_flag(argc, argv, "--strict");
    options->dry_run = workflow_has_flag(argc, argv, "--dry-run");
    options->no_submodules = workflow_has_flag(
        argc, argv, "--no-submodules");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (options->source == NULL) options->source = ".";

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0; index < argc; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(argv[index], "--strict") == 0 ||
            strcmp(argv[index], "--dry-run") == 0 ||
            strcmp(argv[index], "--no-submodules") == 0) continue;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(argv[index], "--source") == 0 ||
            strcmp(argv[index], "--preset") == 0 ||
            strcmp(argv[index], "--jobs") == 0 ||
            strcmp(argv[index], "--message") == 0 ||
            strcmp(argv[index], "--remote") == 0 ||
            strcmp(argv[index], "--branch") == 0 ||
            strcmp(argv[index], "--goal") == 0) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (index + 1 >= argc || argv[index + 1][0] == '\0' ||
                argv[index + 1][0] == '-') {
                (void)fprintf(stderr, "%s requires a value.\n", argv[index]);
                return 0;
            }
            ++index;
            continue;
        }
        (void)fprintf(stderr, "Unknown workflow option: %s\n", argv[index]);
        return 0;
    }
    return 1;
}

/* Provide the workflow goal operation used by this module and its client applications. */
static UmiDeveloperLifecycleGoal workflow_goal(const char *command)
{
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(command, "plan") == 0 || strcmp(command, "verify") == 0)
        return UMI_DEVELOPER_LIFECYCLE_VERIFY;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(command, "build") == 0)
        return UMI_DEVELOPER_LIFECYCLE_BUILD;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(command, "test") == 0)
        return UMI_DEVELOPER_LIFECYCLE_TEST;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(command, "publish") == 0)
        return UMI_DEVELOPER_LIFECYCLE_PUBLISH;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(command, "update") == 0)
        return UMI_DEVELOPER_LIFECYCLE_UPDATE;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(command, "complete") == 0)
        return UMI_DEVELOPER_LIFECYCLE_COMPLETE;
    return 0;
}

/*
 * Provide the workflow print plan operation used by this module and its client
 * applications.
 */
static int workflow_print_plan(
    UmiDeveloperLifecycleGoal goal,
    const UmiCliWorkflowOptions *options)
{
    UmiDeveloperLifecyclePlanRequest request;
    UmiDeveloperLifecyclePlan plan;
    UmiStatus status;
    size_t index;

    umi_developer_lifecycle_plan_request_init(&request, goal);
    request.require_external_vulnerability_scanner = options->strict;
    status = umi_developer_lifecycle_plan_build(&request, &plan);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return 1;
    (void)printf("Umicom %s workflow\n", umi_developer_lifecycle_goal_text(goal));
    (void)printf("Project: %s\n", options->source);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (options->preset != NULL)
        (void)printf("Preset: %s\n", options->preset);
    (void)puts("Stages:");
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < plan.stage_count; ++index) {
        const UmiDeveloperLifecyclePlanStage *stage = &plan.stages[index];
        (void)printf(
            "  %2zu. %-28s %s%s%s\n      %s\n",
            index + 1U,
            stage->title,
            stage->required ? "required" : "optional",
            stage->changes_local_state ? ", changes local state" : "",
            stage->uses_network ? ", uses network" : "",
            stage->explanation);
    }
    return 0;
}

/* Provide the workflow verify operation used by this module and its client applications. */
static int workflow_verify(
    UmiCliContext *context,
    const UmiCliWorkflowOptions *options)
{
    char *check_arguments[3];
    char *quality_arguments[5];
    char *dependency_arguments[3];
    int dependency_count = 2;
    int result;

    check_arguments[0] = "--all";
    check_arguments[1] = "--project";
    check_arguments[2] = (char *)options->source;
    result = umi_cli_command_check(context, 3, check_arguments);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result != 0) return result;

    /* One CI scan covers source defects, security, architecture and static
     * memory categories. This avoids walking a large repository four times. */
    quality_arguments[0] = "scan";
    quality_arguments[1] = (char *)options->source;
    quality_arguments[2] = "--profile";
    quality_arguments[3] = "ci";
    quality_arguments[4] = "--summary";
    result = umi_cli_command_quality(context, 5, quality_arguments);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result != 0) return result;

    dependency_arguments[0] = "audit";
    dependency_arguments[1] = (char *)options->source;
    /* Apply this branch only when its contract condition is satisfied. */
    if (options->strict) {
        dependency_arguments[2] = "--strict";
        dependency_count = 3;
    }
    return umi_cli_command_dependencies(
        context, dependency_count, dependency_arguments);
}

/*
 * Provide the workflow build action operation used by this module and its client
 * applications.
 */
static int workflow_build_action(
    UmiCliContext *context,
    UmiBuildAction action,
    const UmiCliWorkflowOptions *options)
{
    char *arguments[8];
    int count = 0;
    arguments[count++] = "--source";
    arguments[count++] = (char *)options->source;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (options->preset != NULL) {
        arguments[count++] = "--preset";
        arguments[count++] = (char *)options->preset;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (action == UMI_BUILD_COMPILE && options->jobs != NULL) {
        arguments[count++] = "--jobs";
        arguments[count++] = (char *)options->jobs;
    }
    return umi_cli_command_build(context, action, count, arguments);
}

/*
 * Provide the workflow repository verify operation used by this module and its client
 * applications.
 */
static int workflow_repository_verify(
    UmiCliContext *context,
    const UmiCliWorkflowOptions *options)
{
    char *arguments[2];
    arguments[0] = "verify";
    arguments[1] = (char *)options->source;
    return umi_cli_command_repo(context, 2, arguments);
}

/*
 * Provide the workflow repository update operation used by this module and its client
 * applications.
 */
static int workflow_repository_update(
    UmiCliContext *context,
    const UmiCliWorkflowOptions *options)
{
    char *arguments[9];
    int count = 0;
    arguments[count++] = "update";
    arguments[count++] = (char *)options->source;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (options->remote != NULL) {
        arguments[count++] = "--remote";
        arguments[count++] = (char *)options->remote;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (options->branch != NULL) {
        arguments[count++] = "--branch";
        arguments[count++] = (char *)options->branch;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (options->no_submodules) arguments[count++] = "--no-submodules";
    return umi_cli_command_repo(context, count, arguments);
}

/*
 * Provide the workflow repository publish operation used by this module and its client
 * applications.
 */
static int workflow_repository_publish(
    UmiCliContext *context,
    const UmiCliWorkflowOptions *options)
{
    char *arguments[10];
    int count = 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (options->message == NULL) {
        (void)fprintf(stderr, "--message is required for publish.\n");
        return 2;
    }
    arguments[count++] = "publish";
    arguments[count++] = (char *)options->source;
    arguments[count++] = "--message";
    arguments[count++] = (char *)options->message;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (options->remote != NULL) {
        arguments[count++] = "--remote";
        arguments[count++] = (char *)options->remote;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (options->branch != NULL) {
        arguments[count++] = "--branch";
        arguments[count++] = (char *)options->branch;
    }
    return umi_cli_command_repo(context, count, arguments);
}

/*
 * Provide the cli command workflow operation used by this module and its client
 * applications.
 */
int umi_cli_command_workflow(UmiCliContext *context, int argc, char **argv)
{
    UmiCliWorkflowOptions options;
    UmiDeveloperLifecycleGoal goal;
    int result;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (argc < 1 || strcmp(argv[0], "help") == 0 ||
        strcmp(argv[0], "--help") == 0 || strcmp(argv[0], "-h") == 0) {
        (void)puts(
            "Usage: umicom workflow COMMAND [options]\n"
            "Commands: plan, verify, build, test, publish, update, complete\n"
            "Options: --source PATH --preset NAME --jobs N --message TEXT "
            "--remote NAME --branch NAME --strict --dry-run --no-submodules\n\n"
            "Plan accepts --goal verify|build|test|publish|update|complete.\n"
            "build compiles and links. complete verifies, configures, builds, "
            "tests and publishes in that order.");
        return 0;
    }
    goal = workflow_goal(argv[0]);
    /* Apply this branch only when its contract condition is satisfied. */
    if (goal == 0) {
        (void)fprintf(stderr, "Unknown workflow command: %s\n", argv[0]);
        return 2;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!workflow_options_parse(argc - 1, argv + 1, &options)) return 2;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strcmp(argv[0], "plan") == 0 && options.plan_goal != NULL) {
        goal = workflow_goal(options.plan_goal);
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (goal == 0 || strcmp(options.plan_goal, "plan") == 0) {
            (void)fprintf(stderr, "Unknown workflow plan goal: %s\n",
                          options.plan_goal);
            return 2;
        }
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(argv[0], "plan") == 0 || options.dry_run)
        return workflow_print_plan(goal, &options);

    /* Apply this branch only when its contract condition is satisfied. */
    if (goal == UMI_DEVELOPER_LIFECYCLE_UPDATE) {
        result = workflow_repository_verify(context, &options);
        return result == 0
            ? workflow_repository_update(context, &options) : result;
    }

    result = workflow_verify(context, &options);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result != 0) return result;
    /* Apply this branch only when its contract condition is satisfied. */
    if (goal == UMI_DEVELOPER_LIFECYCLE_VERIFY) return 0;

    /* Apply this branch only when its contract condition is satisfied. */
    if (goal == UMI_DEVELOPER_LIFECYCLE_BUILD ||
        goal == UMI_DEVELOPER_LIFECYCLE_TEST ||
        goal == UMI_DEVELOPER_LIFECYCLE_COMPLETE) {
        result = workflow_build_action(context, UMI_BUILD_CONFIGURE, &options);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (result != 0) return result;
        result = workflow_build_action(context, UMI_BUILD_COMPILE, &options);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (result != 0) return result;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (goal == UMI_DEVELOPER_LIFECYCLE_TEST ||
        goal == UMI_DEVELOPER_LIFECYCLE_COMPLETE) {
        result = workflow_build_action(context, UMI_BUILD_TEST, &options);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (result != 0) return result;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (goal == UMI_DEVELOPER_LIFECYCLE_PUBLISH ||
        goal == UMI_DEVELOPER_LIFECYCLE_COMPLETE) {
        result = workflow_repository_verify(context, &options);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (result != 0) return result;
        return workflow_repository_publish(context, &options);
    }
    return 0;
}
