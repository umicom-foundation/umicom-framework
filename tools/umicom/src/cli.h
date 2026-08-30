/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/cli.h
 *
 * PURPOSE:
 *   Share command-line parsing, diagnostics, discovered toolchain state, and
 *   native command handlers for the Umicom application-development command.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOL_CLI_H
#define UMICOM_TOOL_CLI_H

#include "umicom/umicom.h"
#include "umicom/toolchain/operation_types.h"

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
void umi_cli_print_repo_help(void);
void umi_cli_diagnostic_sink(const UmiDiagnostic *diagnostic, void *user_data);
UmiStatus umi_cli_context_prepare(UmiCliContext *context,
                                  const char *project_root,
                                  int require_gtk,
                                  int require_github_cli);
UmiStatus umi_cli_context_prepare_operation(
    UmiCliContext *context,
    const char *project_root,
    UmiToolchainOperationKind operation);
int umi_cli_command_check(UmiCliContext *context, int argc, char **argv);
int umi_cli_command_environment(UmiCliContext *context, int argc, char **argv);
int umi_cli_command_build(UmiCliContext *context,
                          UmiBuildAction action,
                          int argc,
                          char **argv);
int umi_cli_command_repair(UmiCliContext *context, int argc, char **argv);
int umi_cli_command_shell(UmiCliContext *context, int argc, char **argv);
int umi_cli_command_repository(UmiCliContext *context, int argc, char **argv);
int umi_cli_command_repo(UmiCliContext *context, int argc, char **argv);
int umi_cli_command_repository_workflow(UmiCliContext *context,
                                        const char *command,
                                        int argc,
                                        char **argv);
int umi_cli_command_repository_lock(UmiCliContext *context,
                                    int argc,
                                    char **argv);
int umi_cli_command_capabilities(UmiCliContext *context, int argc, char **argv);
int umi_cli_command_suite(UmiCliContext *context, int argc, char **argv);
int umi_cli_command_security(UmiCliContext *context, int argc, char **argv);
int umi_cli_command_architecture(UmiCliContext *context, int argc, char **argv);
int umi_cli_command_quality(UmiCliContext *context, int argc, char **argv);
int umi_cli_command_memory(UmiCliContext *context, int argc, char **argv);
int umi_cli_command_dependencies(UmiCliContext *context, int argc, char **argv);
int umi_cli_command_workflow(UmiCliContext *context, int argc, char **argv);

#endif
