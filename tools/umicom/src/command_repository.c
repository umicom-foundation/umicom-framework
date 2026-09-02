/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_repository.c
 *
 * PURPOSE:
 *   Implement the command repository behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_repository.c
 *
 * PURPOSE:
 *   Implement native repository creation, status and submodule-lock commands.
 *   Mutating workflow commands delegate to the separate reusable workflow
 *   service and do not rely on shell scripts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "cli.h"
#include "umicom/repository/submodule_lock.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Provide the repo option value operation used by this module and its client applications. */
static const char *umi_repo_option_value(
    int argc,
    char **argv,
    const char *option)
{
    int index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0; index + 1 < argc; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(argv[index], option) == 0) {
            return argv[index + 1];
        }
    }
    return NULL;
}

/* Provide the repo has flag operation used by this module and its client applications. */
static int umi_repo_has_flag(
    int argc,
    char **argv,
    const char *option)
{
    int index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0; index < argc; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(argv[index], option) == 0) {
            return 1;
        }
    }
    return 0;
}

/* Provide the repo frontends operation used by this module and its client applications. */
static unsigned umi_repo_frontends(
    const char *text,
    int argc,
    char **argv)
{
    char copy[256];
    char *token;
    char *save_pointer = NULL;
    unsigned frontends = 0U;

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_repo_has_flag(argc, argv, "--console")) {
        frontends |= UMI_FRONTEND_CONSOLE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_repo_has_flag(argc, argv, "--gtk") ||
        umi_repo_has_flag(argc, argv, "--gtk4")) {
        frontends |= UMI_FRONTEND_GTK4;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_repo_has_flag(argc, argv, "--web")) {
        frontends |= UMI_FRONTEND_WEB;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_repo_has_flag(argc, argv, "--qt") ||
        umi_repo_has_flag(argc, argv, "--qt6")) {
        frontends |= UMI_FRONTEND_QT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_repo_has_flag(argc, argv, "--wt")) {
        frontends |= UMI_FRONTEND_WT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_repo_has_flag(argc, argv, "--mobile")) {
        frontends |= UMI_FRONTEND_MOBILE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (frontends != 0U) {
        return frontends;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || text[0] == '\0') {
        return UMI_FRONTEND_CONSOLE | UMI_FRONTEND_GTK4;
    }
    (void)snprintf(copy, sizeof(copy), "%s", text);
    token = strtok_r(copy, ",", &save_pointer);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (token != NULL) {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (strcmp(token, "console") == 0) {
            frontends |= UMI_FRONTEND_CONSOLE;
        } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (strcmp(token, "gtk") == 0 ||
                   strcmp(token, "gtk4") == 0) {
            frontends |= UMI_FRONTEND_GTK4;
        } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (strcmp(token, "web") == 0) {
            frontends |= UMI_FRONTEND_WEB;
        } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (strcmp(token, "qt") == 0 ||
                   strcmp(token, "qt6") == 0) {
            frontends |= UMI_FRONTEND_QT;
        } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (strcmp(token, "wt") == 0) {
            frontends |= UMI_FRONTEND_WT;
        } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (strcmp(token, "mobile") == 0) {
            frontends |= UMI_FRONTEND_MOBILE;
        }
        token = strtok_r(NULL, ",", &save_pointer);
    }
    return frontends;
}

/*
 * Provide the repo framework link operation used by this module and its client
 * applications.
 */
static UmiFrameworkLinkMode umi_repo_framework_link(
    int argc,
    char **argv)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_repo_has_flag(argc, argv, "--framework-installed")) {
        return UMI_FRAMEWORK_LINK_INSTALLED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_repo_has_flag(argc, argv, "--framework-vendored")) {
        return UMI_FRAMEWORK_LINK_VENDORED;
    }
    return UMI_FRAMEWORK_LINK_SUBMODULE;
}

/*
 * Repository dry-run is a planning operation. It must remain usable even when
 * no compiler, GTK installation, GitHub CLI or complete native toolchain is
 * available. The scaffold service validates names and paths before returning
 * from dry-run and does not execute Git/build/remote operations.
 *
 * Real repository creation still performs full toolchain/environment discovery.
 */
static UmiStatus umi_repo_prepare_context(
    UmiCliContext *context,
    int dry_run,
    int require_gtk,
    int require_github_cli)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (dry_run) {
        (void)memset(context, 0, sizeof(*context));
        context->template_root = UMICOM_REPOSITORY_TEMPLATE_ROOT;
        return UMI_STATUS_OK;
    }

    return umi_cli_context_prepare(
        context,
        NULL,
        require_gtk,
        require_github_cli);
}

/*
 * Provide the cli command repository operation used by this module and its client
 * applications.
 */
int umi_cli_command_repository(
    UmiCliContext *context,
    int argc,
    char **argv)
{
    UmiRepositoryScaffoldRequest request;
    UmiRepositoryScaffoldReport report;
    const char *name;
    const char *parent;
    const char *frontends;
    unsigned selected_frontends;
    int option_count;
    char **options;
    int dry_run;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL) {
        (void)fprintf(stderr, "CLI context is required.\n");
        return 2;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (argc < 1) {
        (void)fprintf(stderr, "Repository name is required.\n");
        return 2;
    }

    name = argv[0];
    option_count = argc - 1;
    options = argv + 1;
    parent = umi_repo_option_value(
        option_count, options, "--parent");
    frontends = umi_repo_option_value(
        option_count, options, "--frontends");
    selected_frontends = umi_repo_frontends(
        frontends, option_count, options);
    dry_run = umi_repo_has_flag(
        option_count, options, "--dry-run");

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (parent == NULL) {
        parent = ".";
    }

    status = umi_repo_prepare_context(
        context,
        dry_run,
        (selected_frontends & UMI_FRONTEND_GTK4) != 0U,
        umi_repo_has_flag(option_count, options, "--remote"));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)fprintf(
            stderr,
            "Unable to prepare repository environment: %s\n",
            umi_status_text(status));
        return 1;
    }

    (void)memset(&request, 0, sizeof(request));
    request.template_root = context->template_root;
    request.destination_parent = parent;
    request.application_name = name;
    request.repository_name = umi_repo_option_value(
        option_count, options, "--repo-name");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request.repository_name == NULL) {
        request.repository_name = name;
    }
    request.application_id = umi_repo_option_value(
        option_count, options, "--id");
    request.remote_owner = umi_repo_option_value(
        option_count, options, "--owner");
    request.remote_description = umi_repo_option_value(
        option_count, options, "--description");
    request.framework_url = umi_repo_option_value(
        option_count, options, "--framework-url");
    request.framework_branch = umi_repo_option_value(
        option_count, options, "--framework-branch");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request.framework_branch == NULL) {
        request.framework_branch = "main";
    }

    request.frontends = selected_frontends;
    request.framework_link =
        umi_repo_framework_link(option_count, options);
    request.visibility =
        umi_repo_has_flag(option_count, options, "--public")
            ? UMI_REPOSITORY_PUBLIC
            : UMI_REPOSITORY_PRIVATE;
    request.initialise_git =
        !umi_repo_has_flag(option_count, options, "--no-git");
    request.create_initial_commit =
        request.initialise_git &&
        !umi_repo_has_flag(option_count, options, "--no-commit");
    request.create_remote =
        umi_repo_has_flag(option_count, options, "--remote");
    request.push_initial =
        umi_repo_has_flag(option_count, options, "--push");
    request.dry_run = dry_run;

    /* Apply this branch only when its contract condition is satisfied. */
    if (!request.initialise_git &&
        request.framework_link == UMI_FRAMEWORK_LINK_SUBMODULE) {
        request.framework_link = UMI_FRAMEWORK_LINK_INSTALLED;
    }

    status = umi_repository_scaffold_create(
        &context->discovery.profile,
        &context->environment,
        &request,
        &report);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)fprintf(
            stderr,
            "Repository creation failed: %s\n",
            umi_status_text(status));
        return 1;
    }

    (void)printf("Repository: %s\n", report.repository_root);
    (void)printf("Application: %s\n", report.application_name);
    (void)printf("Application ID: %s\n", report.application_id);
    (void)printf("Files created: %zu\n", report.files_created);
    (void)printf(
        "Directories created: %zu\n",
        report.directories_created);

    /* Apply this branch only when its contract condition is satisfied. */
    if (request.dry_run) {
        (void)puts("Dry run completed; no files changed.");
    }

    return 0;
}

/*
 * Provide the cli command repository lock operation used by this module and its client
 * applications.
 */
int umi_cli_command_repository_lock(
    UmiCliContext *context,
    int argc,
    char **argv)
{
    UmiRepositorySubmoduleLockRequest request;
    UmiRepositorySubmoduleLockReport *report = NULL;
    const char *repository_root = ".";
    int option_start = 0;
    int dry_run = 0;
    int index;
    size_t report_index;
    size_t reported_stage_count;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL) {
        (void)fprintf(stderr, "CLI context is required.\n");
        return 2;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (argc > 0 && argv[0][0] != '-') {
        repository_root = argv[0];
        option_start = 1;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = option_start; index < argc; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(argv[index], "--dry-run") == 0) {
            dry_run = 1;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            (void)fprintf(
                stderr,
                "Unknown repo lock option: %s\n",
                argv[index]);
            return 2;
        }
    }

    /*
     * Lock/verify needs Git, but it does not need CMake, Ninja or a compiler.
     * Operation-scoped preparation prevents an unrelated compile-link-run probe
     * from blocking repository maintenance.
     */
    status = umi_cli_context_prepare_operation(
        context,
        repository_root,
        UMI_TOOLCHAIN_OPERATION_REPOSITORY_WRITE);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)fprintf(
            stderr,
            "Unable to prepare repository environment: %s\n",
            umi_status_text(status));
        return 1;
    }

    report = (UmiRepositorySubmoduleLockReport *)calloc(1U, sizeof(*report));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report == NULL) {
        (void)fprintf(stderr, "Unable to allocate repository lock report.\n");
        return 1;
    }

    (void)memset(&request, 0, sizeof(request));
    request.repository_root = context->project_root;
    request.dry_run = dry_run;

    status = umi_repository_submodule_lock(
        &context->discovery.profile,
        &context->environment,
        &request,
        report);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)fprintf(
            stderr,
            "Repository lock failed: %s\n",
            umi_status_text(status));
        /* Apply this branch only when its contract condition is satisfied. */
        if (report->last_output[0] != '\0') {
            (void)fprintf(stderr, "%s\n", report->last_output);
        }
        free(report);
        return 1;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (report_index = 0U; report_index < report->count; ++report_index) {
        const UmiRepositorySubmoduleLockEntry *entry =
            &report->entries[report_index];
        (void)printf(
            "%s%s -> %s\n",
            dry_run ? "Would lock " : "Locked ",
            entry->path,
            entry->head);
    }

    reported_stage_count = dry_run
        ? report->locked_count
        : report->staged_count;
    (void)printf(
        "%zu submodule%s resolved; %zu gitlink%s %s.\n",
        report->locked_count,
        report->locked_count == 1U ? "" : "s",
        reported_stage_count,
        reported_stage_count == 1U ? "" : "s",
        dry_run ? "would be staged" : "staged");
    /* Apply this branch only when its contract condition is satisfied. */
    if (dry_run) {
        (void)puts("Dry run completed; the parent Git index was not changed.");
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        (void)puts("No commit or push was performed.");
    }

    free(report);
    return 0;
}

/*
 * Provide the cli command repository status operation used by this module and its client
 * applications.
 */
static int umi_cli_command_repository_status(
    UmiCliContext *context,
    int argc,
    char **argv)
{
    const char *repository_root = ".";
    char *output = NULL;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL) {
        (void)fprintf(stderr, "CLI context is required.\n");
        return 2;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (argc > 1) {
        (void)fprintf(stderr, "Usage: umicom repo status [PATH]\n");
        return 2;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (argc == 1) repository_root = argv[0];

    status = umi_cli_context_prepare_operation(
        context, repository_root, UMI_TOOLCHAIN_OPERATION_REPOSITORY_READ);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)fprintf(
            stderr,
            "Unable to prepare repository environment: %s\n",
            umi_status_text(status));
        return 1;
    }

    output = (char *)calloc(UMI_PROCESS_OUTPUT_CAPACITY, sizeof(*output));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (output == NULL) {
        (void)fprintf(stderr, "Unable to allocate repository status buffer.\n");
        return 1;
    }

    status = umi_repository_status(
        &context->discovery.profile,
        &context->environment,
        context->project_root,
        output,
        UMI_PROCESS_OUTPUT_CAPACITY);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)fprintf(
            stderr,
            "Repository status failed: %s\n",
            umi_status_text(status));
        free(output);
        return 1;
    }

    (void)printf("%s", output);
    /* Apply this branch only when its contract condition is satisfied. */
    if (output[0] != '\0' && output[strlen(output) - 1U] != '\n') {
        (void)putchar('\n');
    }
    free(output);
    return 0;
}

/* Provide the cli command repo operation used by this module and its client applications. */
int umi_cli_command_repo(
    UmiCliContext *context,
    int argc,
    char **argv)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL) {
        (void)fprintf(stderr, "CLI context is required.\n");
        return 2;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (argc == 0 || strcmp(argv[0], "help") == 0 ||
        strcmp(argv[0], "--help") == 0 || strcmp(argv[0], "-h") == 0) {
        umi_cli_print_repo_help();
        return 0;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(argv[0], "lock") == 0) {
        return umi_cli_command_repository_lock(
            context, argc - 1, argv + 1);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(argv[0], "verify") == 0) {
        char *verify_argv[2];
        int verify_argc = 1;
        /* Apply this branch only when its contract condition is satisfied. */
        if (argc > 2) {
            (void)fprintf(stderr, "Usage: umicom repo verify [PATH]\n");
            return 2;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (argc == 2) {
            verify_argv[0] = argv[1];
            verify_argv[1] = "--dry-run";
            verify_argc = 2;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            verify_argv[0] = "--dry-run";
        }
        return umi_cli_command_repository_lock(
            context, verify_argc, verify_argv);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(argv[0], "status") == 0) {
        return umi_cli_command_repository_status(
            context, argc - 1, argv + 1);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(argv[0], "audit") == 0 || strcmp(argv[0], "scan") == 0) {
        /* Repository audit reuses the Framework CodeGuard command. This keeps
         * filename, duplication, memory, and architecture policy in one
         * implementation instead of creating a second repository scanner. */
        return umi_cli_command_quality(context, argc - 1, argv + 1);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(argv[0], "create") == 0 || strcmp(argv[0], "new") == 0) {
        return umi_cli_command_repository(context, argc - 1, argv + 1);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(argv[0], "clone") == 0 ||
        strcmp(argv[0], "init") == 0 ||
        strcmp(argv[0], "initialise") == 0 ||
        strcmp(argv[0], "submodule") == 0 ||
        strcmp(argv[0], "stage") == 0 ||
        strcmp(argv[0], "add") == 0 ||
        strcmp(argv[0], "commit") == 0 ||
        strcmp(argv[0], "push") == 0 ||
        strcmp(argv[0], "publish") == 0 ||
        strcmp(argv[0], "update") == 0 ||
        strcmp(argv[0], "sync") == 0) {
        return umi_cli_command_repository_workflow(
            context, argv[0], argc - 1, argv + 1);
    }

    (void)fprintf(stderr, "Unknown repository command: %s\n\n", argv[0]);
    umi_cli_print_repo_help();
    return 2;
}
