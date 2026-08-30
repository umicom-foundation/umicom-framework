/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_repository_workflow.c
 *
 * PURPOSE:
 *   Parse clone, initialise, submodule, stage, commit, push, publish and safe
 *   update commands, then delegate work to the reusable Framework service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/repository/workflow.h"

static const char *umi_cli_repository_option_value(
    int argc,
    char **argv,
    const char *option)
{
    int index;
    for (index = 0; index + 1 < argc; ++index) {
        if (strcmp(argv[index], option) == 0) return argv[index + 1];
    }
    return NULL;
}

static int umi_cli_repository_has_flag(
    int argc,
    char **argv,
    const char *option)
{
    int index;
    for (index = 0; index < argc; ++index) {
        if (strcmp(argv[index], option) == 0) return 1;
    }
    return 0;
}

static int umi_cli_repository_option_allowed(
    const char *value,
    const char *const *flags,
    size_t flag_count,
    const char *const *options,
    size_t option_count,
    int *takes_value)
{
    size_t index;
    for (index = 0U; index < flag_count; ++index) {
        if (strcmp(value, flags[index]) == 0) {
            *takes_value = 0;
            return 1;
        }
    }
    for (index = 0U; index < option_count; ++index) {
        if (strcmp(value, options[index]) == 0) {
            *takes_value = 1;
            return 1;
        }
    }
    return 0;
}

static int umi_cli_repository_validate_options(
    int argc,
    char **argv,
    int option_start,
    const char *const *flags,
    size_t flag_count,
    const char *const *options,
    size_t option_count)
{
    int index;
    for (index = option_start; index < argc; ++index) {
        int takes_value = 0;
        if (!umi_cli_repository_option_allowed(
                argv[index], flags, flag_count,
                options, option_count, &takes_value)) {
            (void)fprintf(stderr, "Unknown repository option: %s\n", argv[index]);
            return 0;
        }
        if (takes_value) {
            if (index + 1 >= argc || argv[index + 1][0] == '\0' ||
                strncmp(argv[index + 1], "--", 2U) == 0) {
                (void)fprintf(stderr, "Option %s requires a value.\n", argv[index]);
                return 0;
            }
            ++index;
        }
    }
    return 1;
}

static UmiStatus umi_cli_repository_prepare_workflow(
    UmiCliContext *context,
    const UmiRepositoryWorkflowRequest *request)
{
    if (context == NULL || request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (request->dry_run) {
        (void)memset(context, 0, sizeof(*context));
        context->template_root = UMICOM_REPOSITORY_TEMPLATE_ROOT;
        return UMI_STATUS_OK;
    }
    /* Clone and push use Git directly. GitHub CLI is required only by the
     * separate remote-repository creation service. */
    return umi_cli_context_prepare_operation(
        context,
        request->repository_root,
        UMI_TOOLCHAIN_OPERATION_REPOSITORY_WRITE);
}

static int umi_cli_repository_run_workflow(
    UmiCliContext *context,
    UmiRepositoryWorkflowRequest *request)
{
    UmiRepositoryWorkflowReport report;
    UmiStatus status;

    status = umi_cli_repository_prepare_workflow(context, request);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr,
                      "Unable to prepare repository tools: %s\n",
                      umi_status_text(status));
        return 1;
    }
    status = umi_repository_workflow_execute(
        request->dry_run ? NULL : &context->discovery.profile,
        request->dry_run ? NULL : &context->environment,
        request,
        &report);

    (void)printf("Repository action: %s\n",
                 umi_repository_workflow_action_text(request->action));
    (void)printf("Repository: %s\n", request->repository_root);
    if (report.output[0] != '\0') {
        (void)printf("%s%s", report.output,
                     report.output[strlen(report.output) - 1U] == '\n'
                         ? "" : "\n");
    }
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr,
                      "Repository action failed: %s",
                      umi_status_text(status));
        if (report.last_exit_code >= 0) {
            (void)fprintf(stderr,
                          " (Git exit code %d)",
                          report.last_exit_code);
        }
        (void)fputc('\n', stderr);
        return 1;
    }
    if (report.planned) (void)puts("Dry run completed; no files changed.");
    if (report.no_changes) (void)puts("No staged content required a new commit.");
    if (report.commit_created) (void)puts("Commit created.");
    if (report.pushed) (void)puts("Push completed.");
    if (report.fetched) (void)puts("Remote revisions fetched.");
    if (report.updated) (void)puts("Repository updated by fast-forward.");
    if (report.submodules_updated) (void)puts("Submodules synchronised.");
    return 0;
}

static int umi_cli_repository_clone(
    UmiCliContext *context,
    int argc,
    char **argv)
{
    static const char *const flags[] = {"--recursive", "--no-recursive", "--dry-run"};
    static const char *const options[] = {"--root", "--branch", "--depth"};
    UmiRepositoryWorkflowRequest request;
    const char *depth_text;
    char *depth_end = NULL;
    unsigned long depth = 0UL;

    if (argc < 2) {
        (void)fprintf(stderr,
                      "Usage: umicom repo clone URL DESTINATION [options]\n");
        return 2;
    }
    if (!umi_cli_repository_validate_options(
            argc, argv, 2,
            flags, sizeof(flags) / sizeof(flags[0]),
            options, sizeof(options) / sizeof(options[0]))) {
        return 2;
    }
    depth_text = umi_cli_repository_option_value(argc, argv, "--depth");
    if (depth_text != NULL) {
        depth = strtoul(depth_text, &depth_end, 10);
        if (depth == 0UL || depth_end == NULL || *depth_end != '\0' ||
            depth > 1000000UL) {
            (void)fprintf(stderr, "--depth must be a positive whole number.\n");
            return 2;
        }
    }
    umi_repository_workflow_request_init(
        &request,
        UMI_REPOSITORY_WORKFLOW_CLONE,
        umi_cli_repository_option_value(argc, argv, "--root"));
    request.source_url = argv[0];
    request.destination = argv[1];
    request.branch = umi_cli_repository_option_value(argc, argv, "--branch");
    request.clone_depth = (unsigned)depth;
    request.recursive = !umi_cli_repository_has_flag(
        argc, argv, "--no-recursive");
    request.dry_run = umi_cli_repository_has_flag(argc, argv, "--dry-run");
    return umi_cli_repository_run_workflow(context, &request);
}

static int umi_cli_repository_init(
    UmiCliContext *context,
    int argc,
    char **argv)
{
    static const char *const flags[] = {"--dry-run"};
    static const char *const options[] = {
        "--branch", "--remote", "--remote-url"
    };
    UmiRepositoryWorkflowRequest request;
    const char *root = argc > 0 && argv[0][0] != '-' ? argv[0] : ".";
    int option_start = argc > 0 && argv[0][0] != '-' ? 1 : 0;

    if (!umi_cli_repository_validate_options(
            argc, argv, option_start,
            flags, sizeof(flags) / sizeof(flags[0]),
            options, sizeof(options) / sizeof(options[0]))) {
        return 2;
    }
    umi_repository_workflow_request_init(
        &request, UMI_REPOSITORY_WORKFLOW_INITIALISE, root);
    if (umi_cli_repository_option_value(argc, argv, "--branch") != NULL) {
        request.branch = umi_cli_repository_option_value(argc, argv, "--branch");
    }
    if (umi_cli_repository_option_value(argc, argv, "--remote") != NULL) {
        request.remote_name = umi_cli_repository_option_value(argc, argv, "--remote");
    }
    request.remote_url = umi_cli_repository_option_value(argc, argv, "--remote-url");
    request.dry_run = umi_cli_repository_has_flag(argc, argv, "--dry-run");
    return umi_cli_repository_run_workflow(context, &request);
}

static int umi_cli_repository_submodule(
    UmiCliContext *context,
    int argc,
    char **argv)
{
    static const char *const flags[] = {"--dry-run"};
    static const char *const options[] = {"--root", "--branch"};
    UmiRepositoryWorkflowRequest request;

    if (argc < 3 || strcmp(argv[0], "add") != 0) {
        (void)fprintf(stderr,
                      "Usage: umicom repo submodule add URL PATH [options]\n");
        return 2;
    }
    if (!umi_cli_repository_validate_options(
            argc, argv, 3,
            flags, sizeof(flags) / sizeof(flags[0]),
            options, sizeof(options) / sizeof(options[0]))) {
        return 2;
    }
    umi_repository_workflow_request_init(
        &request,
        UMI_REPOSITORY_WORKFLOW_SUBMODULE_ADD,
        umi_cli_repository_option_value(argc, argv, "--root"));
    request.source_url = argv[1];
    request.submodule_path = argv[2];
    if (umi_cli_repository_option_value(argc, argv, "--branch") != NULL) {
        request.branch = umi_cli_repository_option_value(argc, argv, "--branch");
    }
    request.dry_run = umi_cli_repository_has_flag(argc, argv, "--dry-run");
    return umi_cli_repository_run_workflow(context, &request);
}

static int umi_cli_repository_simple(
    UmiCliContext *context,
    UmiRepositoryWorkflowAction action,
    int argc,
    char **argv)
{
    static const char *const dry_run_flag[] = {"--dry-run"};
    static const char *const push_flags[] = {"--set-upstream", "--dry-run"};
    static const char *const message_option[] = {"--message"};
    static const char *const push_options[] = {"--remote", "--branch"};
    static const char *const publish_options[] = {
        "--message", "--remote", "--branch"
    };
    static const char *const update_flags[] = {
        "--dry-run", "--no-submodules"
    };
    const char *const *flags = dry_run_flag;
    const char *const *options = NULL;
    size_t flag_count = sizeof(dry_run_flag) / sizeof(dry_run_flag[0]);
    size_t option_count = 0U;
    UmiRepositoryWorkflowRequest request;
    const char *root = argc > 0 && argv[0][0] != '-' ? argv[0] : ".";
    int option_start = argc > 0 && argv[0][0] != '-' ? 1 : 0;

    if (action == UMI_REPOSITORY_WORKFLOW_COMMIT) {
        options = message_option;
        option_count = sizeof(message_option) / sizeof(message_option[0]);
    } else if (action == UMI_REPOSITORY_WORKFLOW_PUSH) {
        flags = push_flags;
        flag_count = sizeof(push_flags) / sizeof(push_flags[0]);
        options = push_options;
        option_count = sizeof(push_options) / sizeof(push_options[0]);
    } else if (action == UMI_REPOSITORY_WORKFLOW_PUBLISH) {
        flags = push_flags;
        flag_count = sizeof(push_flags) / sizeof(push_flags[0]);
        options = publish_options;
        option_count = sizeof(publish_options) / sizeof(publish_options[0]);
    } else if (action == UMI_REPOSITORY_WORKFLOW_UPDATE) {
        flags = update_flags;
        flag_count = sizeof(update_flags) / sizeof(update_flags[0]);
        options = push_options;
        option_count = sizeof(push_options) / sizeof(push_options[0]);
    }
    if (!umi_cli_repository_validate_options(
            argc, argv, option_start,
            flags, flag_count, options, option_count)) {
        return 2;
    }
    umi_repository_workflow_request_init(&request, action, root);
    request.commit_message = umi_cli_repository_option_value(
        argc, argv, "--message");
    if ((action == UMI_REPOSITORY_WORKFLOW_COMMIT ||
         action == UMI_REPOSITORY_WORKFLOW_PUBLISH) &&
        request.commit_message == NULL) {
        (void)fprintf(stderr, "--message is required for this command.\n");
        return 2;
    }
    if (umi_cli_repository_option_value(argc, argv, "--remote") != NULL) {
        request.remote_name = umi_cli_repository_option_value(argc, argv, "--remote");
    }
    if (umi_cli_repository_option_value(argc, argv, "--branch") != NULL) {
        request.branch = umi_cli_repository_option_value(argc, argv, "--branch");
    }
    request.set_upstream = umi_cli_repository_has_flag(
        argc, argv, "--set-upstream");
    request.recursive = !umi_cli_repository_has_flag(
        argc, argv, "--no-submodules");
    request.dry_run = umi_cli_repository_has_flag(argc, argv, "--dry-run");
    return umi_cli_repository_run_workflow(context, &request);
}

int umi_cli_command_repository_workflow(
    UmiCliContext *context,
    const char *command,
    int argc,
    char **argv)
{
    if (command == NULL) return 2;
    if (strcmp(command, "clone") == 0) {
        return umi_cli_repository_clone(context, argc, argv);
    }
    if (strcmp(command, "init") == 0 || strcmp(command, "initialise") == 0) {
        return umi_cli_repository_init(context, argc, argv);
    }
    if (strcmp(command, "submodule") == 0) {
        return umi_cli_repository_submodule(context, argc, argv);
    }
    if (strcmp(command, "stage") == 0 || strcmp(command, "add") == 0) {
        return umi_cli_repository_simple(
            context, UMI_REPOSITORY_WORKFLOW_STAGE, argc, argv);
    }
    if (strcmp(command, "commit") == 0) {
        return umi_cli_repository_simple(
            context, UMI_REPOSITORY_WORKFLOW_COMMIT, argc, argv);
    }
    if (strcmp(command, "push") == 0) {
        return umi_cli_repository_simple(
            context, UMI_REPOSITORY_WORKFLOW_PUSH, argc, argv);
    }
    if (strcmp(command, "publish") == 0) {
        return umi_cli_repository_simple(
            context, UMI_REPOSITORY_WORKFLOW_PUBLISH, argc, argv);
    }
    if (strcmp(command, "update") == 0 || strcmp(command, "sync") == 0) {
        return umi_cli_repository_simple(
            context, UMI_REPOSITORY_WORKFLOW_UPDATE, argc, argv);
    }
    return 2;
}
