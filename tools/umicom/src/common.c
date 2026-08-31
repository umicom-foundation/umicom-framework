/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/common.c
 *
 * PURPOSE:
 *   Implement shared native CLI help, diagnostics, root detection, toolchain
 *   discovery, dependency validation, and child-process environment planning.
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

#include "umicom/toolchain/operation_context.h"

void umi_cli_print_help(void)
{
    (void)puts(
        "Umicom Framework native command\n\n"
        "Usage:\n"
        "  umicom version\n"
        "  umicom check [--all] [--gtk] [--github] [--project PATH]\n"
        "  umicom env\n"
        "  umicom repair [--dry-run]\n"
        "  umicom shell [powershell.exe|cmd.exe|/bin/sh]\n"
        "  umicom configure [--source PATH] [--preset NAME | --build PATH]\n"
        "  umicom build [--source PATH] [--preset NAME | --build PATH] [--target TARGET] [--jobs N]\n"
        "  umicom test [--source PATH] [--preset NAME | --build PATH]\n"
        "  umicom clean [--source PATH] [--preset NAME | --build PATH]\n"
        "  umicom install [--source PATH] [--preset NAME] [--prefix PATH]\n"
        "  umicom package [--source PATH] [--preset NAME] [--jobs N]\n"
        "  umicom deliver [--source PATH] [--preset NAME] [--prefix PATH]\n"
        "  umicom make [--source PATH] [--preset NAME | --build PATH] [--jobs N]\n"
        "  umicom run studio [--source PATH] [--preset NAME]\n"
        "  umicom run --executable PATH [--source PATH] [--preset NAME]\n"
        "  umicom dev doctor|configure|build|test|clean|install|package|deliver|run\n"
        "  umicom quality scan [PATH] [--profile NAME] [--format NAME]\n"
        "  umicom security scan [PATH]\n"
        "  umicom architecture check [PATH]\n"
        "  umicom memory scan [PATH]\n"
        "  umicom dependencies inventory [PATH]\n"
        "  umicom dependencies audit [PATH] [--strict] [--dry-run]\n"
        "  umicom workflow plan|verify|build|test|publish|update|complete [options]\n"
        "  umicom repo help\n"
        "  umicom repo status [PATH]\n"
        "  umicom repo audit [PATH] [quality options]\n"
        "  umicom repo verify [PATH]\n"
        "  umicom repo lock [PATH] [--dry-run]\n"
        "  umicom repo create NAME [repository options]\n"
        "  umicom repo clone URL DESTINATION [--root PATH] [--branch NAME] [--depth N]\n"
        "  umicom repo init [PATH] [--branch NAME] [--remote-url URL]\n"
        "  umicom repo submodule add URL PATH [--root PATH] [--branch NAME]\n"
        "  umicom repo stage|add [PATH] [--dry-run]\n"
        "  umicom repo commit [PATH] --message MESSAGE [--dry-run]\n"
        "  umicom repo push [PATH] [--remote NAME] [--branch NAME]\n"
        "  umicom repo publish [PATH] --message MESSAGE [push options]\n"
        "  umicom repo update [PATH] [--remote NAME] [--branch NAME]\n"
        "  umicom status|add|commit|push ... (familiar repository aliases)\n"
        "  umicom create repo NAME [options]\n"
        "  umicom new repo NAME [options]\n"
        "  umicom capabilities\n"
        "  umicom suite sample PATH\n\n"
        "Repository lock:\n"
        "  PATH                      Parent repository root (default: .)\n"
        "  --dry-run                 Resolve HEADs without staging gitlinks\n"
        "  The lock command never commits and never pushes.\n\n"
        "Repository options:\n"
        "  --parent PATH             Parent directory for the new repository\n"
        "  --repo-name NAME          Repository/folder name (default: app name)\n"
        "  --id APPLICATION_ID       Stable application identifier\n"
        "  --frontends LIST          console,gtk,web,qt,wt,mobile\n"
        "  --console --gtk --web     Friendly individual frontend flags\n"
        "  --framework-url URL       Framework Git submodule URL\n"
        "  --framework-branch NAME   Framework branch (default: main)\n"
        "  --framework-installed     Use an installed Framework package\n"
        "  --framework-vendored      Reserved vendored Framework mode\n"
        "  --no-git --no-commit      Skip local Git or initial commit\n"
        "  --owner OWNER             GitHub organisation or user\n"
        "  --public | --private      Remote visibility\n"
        "  --remote                  Create the GitHub repository with gh\n"
        "  --push                    Push the initial commit\n"
        "  --dry-run                 Display plan without changing files\n"
        "\nRepository workflow safety:\n"
        "  publish runs add -A, status, diff --check, a non-empty commit, and push.\n"
        "  --dry-run plans a mutation without changing local or remote state.\n"
        "  Index locks are reported but never deleted automatically.\n"
        "  Force push is not supported by the safe workflow.\n"
    );
}


void umi_cli_print_repo_help(void)
{
    (void)puts(
        "Umicom Framework native repository commands\n\n"
        "Usage:\n"
        "  umicom repo help\n"
        "  umicom repo status [PATH]\n"
        "  umicom repo audit [PATH] [--summary]\n"
        "  umicom repo verify [PATH]\n"
        "  umicom repo lock [PATH] [--dry-run]\n"
        "  umicom repo create NAME [options]\n"
        "  umicom repo clone URL DESTINATION [options]\n"
        "  umicom repo init [PATH] [options]\n"
        "  umicom repo submodule add URL PATH [options]\n"
        "  umicom repo stage|add [PATH] [-A] [--dry-run]\n"
        "  umicom repo commit [PATH] -m MESSAGE [--dry-run]\n"
        "  umicom repo push [PATH] [options]\n"
        "  umicom repo publish [PATH] --message MESSAGE [options]\n\n"
        "  umicom repo update [PATH] [options]\n\n"
        "Common workflow options:\n"
        "  --branch NAME             Branch to clone, initialise or push\n"
        "  --remote NAME             Remote name (default: origin)\n"
        "  --set-upstream            Record the pushed branch as upstream\n"
        "  --dry-run                 Validate and display the plan only\n\n"
        "Familiar aliases:\n"
        "  umicom add -A\n"
        "  umicom status\n"
        "  umicom commit -m \"Describe the change\"\n"
        "  umicom push\n\n"
        "Safety:\n"
        "  repo audit is read-only and reports naming, duplication, and quality findings.\n"
        "  repo verify is a non-mutating lock validation pass.\n"
        "  repo lock stages only configured submodule gitlinks.\n"
        "  repo lock never creates a commit and never pushes.\n"
        "  repo publish skips empty commits and never force-pushes.\n"
        "  repo update requires a clean tracked worktree and fast-forwards only.\n"
        "  index.lock is explained but never removed automatically.\n"
    );
}

void umi_cli_diagnostic_sink(const UmiDiagnostic *diagnostic, void *user_data)
{
    (void)user_data;
    if (diagnostic == NULL) return;
    (void)fprintf(stderr,
                  "[%s][%s] %s\n",
                  umi_diagnostic_severity_text(diagnostic->severity),
                  diagnostic->source != NULL ? diagnostic->source : "umicom",
                  diagnostic->message != NULL ? diagnostic->message : "");
}

UmiStatus umi_cli_context_prepare(UmiCliContext *context,
                                  const char *project_root,
                                  int require_gtk,
                                  int require_github_cli)
{
    UmiToolchainDiscoveryRequest request;
    UmiStatus status;

    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(context, 0, sizeof(*context));
    context->template_root = UMICOM_REPOSITORY_TEMPLATE_ROOT;
    if (project_root != NULL && project_root[0] != '\0') {
        (void)snprintf(context->project_root,
                       sizeof(context->project_root),
                       "%s",
                       project_root);
    } else {
        status = umi_fs_current_directory(context->project_root,
                                          sizeof(context->project_root));
        if (status != UMI_STATUS_OK) return status;
    }

    (void)memset(&request, 0, sizeof(request));
    request.require_gtk = require_gtk;
    request.require_github_cli = require_github_cli;
    request.diagnostic_sink = umi_cli_diagnostic_sink;
    status = umi_toolchain_discover(&request, &context->discovery);
    if (status != UMI_STATUS_OK) return status;
    status = umi_environment_plan_from_toolchain(&context->discovery.profile,
                                                 &context->environment);
    if (status == UMI_STATUS_OK) context->environment_ready = 1;
    return status;
}


/*
 * Prepare only the native tools required by one operation.
 *
 * Repository commands intentionally use this path instead of the historical
 * full development-environment preparation. A Git status or submodule lock is
 * not a compiler operation and must not fail because a C compile-link-run probe
 * is unavailable. Build/configure/test commands continue to use the established
 * full preparation path unless explicitly migrated to an operation profile.
 */
UmiStatus umi_cli_context_prepare_operation(
    UmiCliContext *context,
    const char *project_root,
    UmiToolchainOperationKind operation)
{
    UmiToolchainOperationContext *operation_context = NULL;
    UmiStatus status;

    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(context, 0, sizeof(*context));
    context->template_root = UMICOM_REPOSITORY_TEMPLATE_ROOT;

    if (project_root != NULL && project_root[0] != '\0') {
        if (strlen(project_root) >= sizeof(context->project_root)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)snprintf(context->project_root, sizeof(context->project_root), "%s", project_root);
    } else {
        status = umi_fs_current_directory(context->project_root, sizeof(context->project_root));
        if (status != UMI_STATUS_OK) return status;
    }

    operation_context = (UmiToolchainOperationContext *)calloc(1U, sizeof(*operation_context));
    if (operation_context == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    status = umi_toolchain_operation_context_prepare(
        operation,
        NULL,
        NULL,
        umi_cli_diagnostic_sink,
        NULL,
        operation_context);
    if (status == UMI_STATUS_OK) {
        context->discovery.profile = operation_context->discovery.profile;
        context->discovery.tools_found = operation_context->discovery.tools_found;
        context->discovery.required_tools = operation_context->discovery.requirement_count +
            (operation_context->operation.requires_compiler ? 1U : 0U);
        context->discovery.required_tools_missing = operation_context->discovery.required_missing;
        context->discovery.compile_probe_passed = operation_context->discovery.compile_probe_passed;
        context->discovery.link_probe_passed = operation_context->discovery.compile_probe_passed;
        context->discovery.runtime_probe_passed = operation_context->discovery.compile_probe_passed;
        context->discovery.c23_probe_passed = operation_context->discovery.compile_probe_passed;
        context->environment = operation_context->environment;
        context->environment_ready = 1;
    }

    free(operation_context);
    return status;
}
