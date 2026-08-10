/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/cli.h
 *
 * PURPOSE:
 *   Share command-line parsing, diagnostics, discovered toolchain state, and
 *   native command handlers for the Umicom application-development command.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOL_CLI_H
#define UMICOM_TOOL_CLI_H

#include "umicom/umicom.h"

#ifndef UMICOM_REPOSITORY_TEMPLATE_ROOT
#define UMICOM_REPOSITORY_TEMPLATE_ROOT "templates/repository"
#endif

typedef struct UmiCliContext {
    char project_root[UMI_PATH_CAPACITY];
    const char *template_root;
    UmiToolchainDiscoveryReport discovery;
    UmiEnvironmentPlan environment;
    int environment_ready;
} UmiCliContext;

void umi_cli_print_help(void);
void umi_cli_diagnostic_sink(const UmiDiagnostic *diagnostic, void *user_data);
UmiStatus umi_cli_context_prepare(UmiCliContext *context,
                                  const char *project_root,
                                  int require_gtk,
                                  int require_github_cli);
int umi_cli_command_check(UmiCliContext *context, int argc, char **argv);
int umi_cli_command_environment(UmiCliContext *context, int argc, char **argv);
int umi_cli_command_build(UmiCliContext *context,
                          UmiBuildAction action,
                          int argc,
                          char **argv);
int umi_cli_command_repair(UmiCliContext *context, int argc, char **argv);
int umi_cli_command_shell(UmiCliContext *context, int argc, char **argv);
int umi_cli_command_repository(UmiCliContext *context, int argc, char **argv);
int umi_cli_command_capabilities(UmiCliContext *context, int argc, char **argv);
int umi_cli_command_suite(UmiCliContext *context, int argc, char **argv);

#endif
