/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/common.c
 *
 * PURPOSE:
 *   Implement shared native CLI help, diagnostics, root detection, toolchain
 *   discovery, dependency validation, and child-process environment planning.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "cli.h"

#include <stdio.h>
#include <string.h>

void umi_cli_print_help(void)
{
    (void)puts(
        "Umicom Framework native command\n\n"
        "Usage:\n"
        "  umicom version\n"
        "  umicom check [--gtk] [--github]\n"
        "  umicom env\n"
        "  umicom repair [--dry-run]\n"
        "  umicom shell [powershell.exe|cmd.exe|/bin/sh]\n"
        "  umicom configure [--source PATH] [--preset NAME | --build PATH]\n"
        "  umicom build [--source PATH] [--preset NAME | --build PATH] [--target TARGET] [--jobs N]\n"
        "  umicom test [--source PATH] [--preset NAME | --build PATH]\n"
        "  umicom make [--source PATH] [--preset NAME | --build PATH] [--jobs N]\n"
        "  umicom run studio [--source PATH] [--preset NAME]\n"
        "  umicom run --executable PATH [--source PATH] [--preset NAME]\n"
        "  umicom create repo NAME [options]\n"
        "  umicom new repo NAME [options]\n"
        "  umicom capabilities\n"
        "  umicom suite sample PATH\n\n"
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
