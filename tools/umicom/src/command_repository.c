#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_repository.c
 *
 * PURPOSE:
 *   Implement `umicom create repo` and `umicom new repo`, including application
 *   frontends, local Git initialisation, Framework submodule linking, optional
 *   GitHub remote creation, initial commit, and explicit push controls.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "cli.h"

#include <stdio.h>
#include <string.h>

static const char *umi_repo_option_value(
    int argc,
    char **argv,
    const char *option)
{
    int index;

    for (index = 0; index + 1 < argc; ++index) {
        if (strcmp(argv[index], option) == 0) {
            return argv[index + 1];
        }
    }
    return NULL;
}

static int umi_repo_has_flag(
    int argc,
    char **argv,
    const char *option)
{
    int index;

    for (index = 0; index < argc; ++index) {
        if (strcmp(argv[index], option) == 0) {
            return 1;
        }
    }
    return 0;
}

static unsigned umi_repo_frontends(
    const char *text,
    int argc,
    char **argv)
{
    char copy[256];
    char *token;
    char *save_pointer = NULL;
    unsigned frontends = 0U;

    if (umi_repo_has_flag(argc, argv, "--console")) {
        frontends |= UMI_FRONTEND_CONSOLE;
    }
    if (umi_repo_has_flag(argc, argv, "--gtk") ||
        umi_repo_has_flag(argc, argv, "--gtk4")) {
        frontends |= UMI_FRONTEND_GTK4;
    }
    if (umi_repo_has_flag(argc, argv, "--web")) {
        frontends |= UMI_FRONTEND_WEB;
    }
    if (umi_repo_has_flag(argc, argv, "--qt") ||
        umi_repo_has_flag(argc, argv, "--qt6")) {
        frontends |= UMI_FRONTEND_QT;
    }
    if (umi_repo_has_flag(argc, argv, "--wt")) {
        frontends |= UMI_FRONTEND_WT;
    }
    if (umi_repo_has_flag(argc, argv, "--mobile")) {
        frontends |= UMI_FRONTEND_MOBILE;
    }
    if (frontends != 0U) {
        return frontends;
    }

    if (text == NULL || text[0] == '\0') {
        return UMI_FRONTEND_CONSOLE | UMI_FRONTEND_GTK4;
    }
    (void)snprintf(copy, sizeof(copy), "%s", text);
    token = strtok_r(copy, ",", &save_pointer);
    while (token != NULL) {
        if (strcmp(token, "console") == 0) {
            frontends |= UMI_FRONTEND_CONSOLE;
        } else if (strcmp(token, "gtk") == 0 ||
                   strcmp(token, "gtk4") == 0) {
            frontends |= UMI_FRONTEND_GTK4;
        } else if (strcmp(token, "web") == 0) {
            frontends |= UMI_FRONTEND_WEB;
        } else if (strcmp(token, "qt") == 0 ||
                   strcmp(token, "qt6") == 0) {
            frontends |= UMI_FRONTEND_QT;
        } else if (strcmp(token, "wt") == 0) {
            frontends |= UMI_FRONTEND_WT;
        } else if (strcmp(token, "mobile") == 0) {
            frontends |= UMI_FRONTEND_MOBILE;
        }
        token = strtok_r(NULL, ",", &save_pointer);
    }
    return frontends;
}

static UmiFrameworkLinkMode umi_repo_framework_link(
    int argc,
    char **argv)
{
    if (umi_repo_has_flag(argc, argv, "--framework-installed")) {
        return UMI_FRAMEWORK_LINK_INSTALLED;
    }
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
    if (context == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

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

    if (context == NULL) {
        (void)fprintf(stderr, "CLI context is required.\n");
        return 2;
    }
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

    if (parent == NULL) {
        parent = ".";
    }

    status = umi_repo_prepare_context(
        context,
        dry_run,
        (selected_frontends & UMI_FRONTEND_GTK4) != 0U,
        umi_repo_has_flag(option_count, options, "--remote"));
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

    if (!request.initialise_git &&
        request.framework_link == UMI_FRAMEWORK_LINK_SUBMODULE) {
        request.framework_link = UMI_FRAMEWORK_LINK_INSTALLED;
    }

    status = umi_repository_scaffold_create(
        &context->discovery.profile,
        &context->environment,
        &request,
        &report);
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

    if (request.dry_run) {
        (void)puts("Dry run completed; no files changed.");
    }

    return 0;
}
