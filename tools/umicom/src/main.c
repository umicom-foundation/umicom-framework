/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/main.c
 *
 * PURPOSE:
 *   Provide the native Umicom command used to inspect and repair development
 *   environments, configure, build, test and run applications, create local or
 *   remote repositories, scaffold applications, and compose suites.
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
#include <string.h>

static int umi_cli_command_dev(
    UmiCliContext *context,
    int argc,
    char **argv)
{
    if (argc < 1 || strcmp(argv[0], "help") == 0 ||
        strcmp(argv[0], "--help") == 0) {
        (void)puts(
            "Usage: umicom dev COMMAND [options]\n"
            "Commands: doctor, configure, build, test, clean, install, "
            "package, deliver, run, workflow");
        return 0;
    }
    if (strcmp(argv[0], "doctor") == 0 || strcmp(argv[0], "check") == 0) {
        return umi_cli_command_check(context, argc - 1, argv + 1);
    }
    if (strcmp(argv[0], "configure") == 0) {
        return umi_cli_command_build(
            context, UMI_BUILD_CONFIGURE, argc - 1, argv + 1);
    }
    if (strcmp(argv[0], "build") == 0) {
        return umi_cli_command_build(
            context, UMI_BUILD_COMPILE, argc - 1, argv + 1);
    }
    if (strcmp(argv[0], "test") == 0) {
        return umi_cli_command_build(
            context, UMI_BUILD_TEST, argc - 1, argv + 1);
    }
    if (strcmp(argv[0], "clean") == 0) {
        return umi_cli_command_build(
            context, UMI_BUILD_CLEAN, argc - 1, argv + 1);
    }
    if (strcmp(argv[0], "install") == 0) {
        return umi_cli_command_build(
            context, UMI_BUILD_INSTALL, argc - 1, argv + 1);
    }
    if (strcmp(argv[0], "package") == 0) {
        return umi_cli_command_build(
            context, UMI_BUILD_PACKAGE, argc - 1, argv + 1);
    }
    if (strcmp(argv[0], "deliver") == 0) {
        return umi_cli_command_build(
            context, UMI_BUILD_DELIVER, argc - 1, argv + 1);
    }
    if (strcmp(argv[0], "run") == 0) {
        return umi_cli_command_build(
            context, UMI_BUILD_RUN, argc - 1, argv + 1);
    }
    if (strcmp(argv[0], "workflow") == 0) {
        return umi_cli_command_workflow(context, argc - 1, argv + 1);
    }
    (void)fprintf(stderr, "Unknown developer command: %s\n", argv[0]);
    return 2;
}

int main(int argc, char **argv)
{
    UmiCliContext context;
    const char *command;

    if (argc < 2) {
        umi_cli_print_help();
        return 0;
    }
    command = argv[1];
    if (strcmp(command, "help") == 0 || strcmp(command, "--help") == 0 ||
        strcmp(command, "-h") == 0) {
        umi_cli_print_help();
        return 0;
    }
    if (strcmp(command, "version") == 0 || strcmp(command, "--version") == 0) {
        (void)printf("Umicom Framework %s (ABI %u)\n",
                     UMICOM_FRAMEWORK_VERSION_STRING,
                     UMICOM_FRAMEWORK_ABI_VERSION);
        return 0;
    }
    if (strcmp(command, "check") == 0 || strcmp(command, "doctor") == 0)
        return umi_cli_command_check(&context, argc - 2, argv + 2);
    if (strcmp(command, "env") == 0)
        return umi_cli_command_environment(&context, argc - 2, argv + 2);
    if (strcmp(command, "repair") == 0)
        return umi_cli_command_repair(&context, argc - 2, argv + 2);
    if (strcmp(command, "shell") == 0)
        return umi_cli_command_shell(&context, argc - 2, argv + 2);
    if (strcmp(command, "configure") == 0)
        return umi_cli_command_build(&context,
                                     UMI_BUILD_CONFIGURE,
                                     argc - 2,
                                     argv + 2);
    if (strcmp(command, "build") == 0)
        return umi_cli_command_build(&context,
                                     UMI_BUILD_COMPILE,
                                     argc - 2,
                                     argv + 2);
    if (strcmp(command, "test") == 0)
        return umi_cli_command_build(&context,
                                     UMI_BUILD_TEST,
                                     argc - 2,
                                     argv + 2);
    if (strcmp(command, "clean") == 0)
        return umi_cli_command_build(&context,
                                     UMI_BUILD_CLEAN,
                                     argc - 2,
                                     argv + 2);
    if (strcmp(command, "install") == 0)
        return umi_cli_command_build(&context,
                                     UMI_BUILD_INSTALL,
                                     argc - 2,
                                     argv + 2);
    if (strcmp(command, "package") == 0)
        return umi_cli_command_build(&context,
                                     UMI_BUILD_PACKAGE,
                                     argc - 2,
                                     argv + 2);
    if (strcmp(command, "deliver") == 0)
        return umi_cli_command_build(&context,
                                     UMI_BUILD_DELIVER,
                                     argc - 2,
                                     argv + 2);
    if (strcmp(command, "make") == 0)
        return umi_cli_command_build(&context,
                                     UMI_BUILD_MAKE,
                                     argc - 2,
                                     argv + 2);
    if (strcmp(command, "run") == 0)
        return umi_cli_command_build(&context,
                                     UMI_BUILD_RUN,
                                     argc - 2,
                                     argv + 2);
    if (strcmp(command, "dev") == 0)
        return umi_cli_command_dev(&context, argc - 2, argv + 2);
    if (strcmp(command, "repo") == 0)
        return umi_cli_command_repo(&context, argc - 2, argv + 2);
    if ((strcmp(command, "create") == 0 || strcmp(command, "new") == 0) &&
        argc >= 3 && strcmp(argv[2], "repo") == 0)
        return umi_cli_command_repository(&context, argc - 3, argv + 3);
    if (strcmp(command, "capabilities") == 0)
        return umi_cli_command_capabilities(&context, argc - 2, argv + 2);
    if (strcmp(command, "suite") == 0)
        return umi_cli_command_suite(&context, argc - 2, argv + 2);
    if (strcmp(command, "security") == 0)
        return umi_cli_command_security(&context, argc - 2, argv + 2);
    if (strcmp(command, "architecture") == 0)
        return umi_cli_command_architecture(&context, argc - 2, argv + 2);
    if (strcmp(command, "quality") == 0 || strcmp(command, "scan") == 0)
        return umi_cli_command_quality(&context, argc - 2, argv + 2);
    if (strcmp(command, "memory") == 0)
        return umi_cli_command_memory(&context, argc - 2, argv + 2);
    if (strcmp(command, "dependencies") == 0 || strcmp(command, "deps") == 0)
        return umi_cli_command_dependencies(&context, argc - 2, argv + 2);
    if (strcmp(command, "workflow") == 0)
        return umi_cli_command_workflow(&context, argc - 2, argv + 2);

    (void)fprintf(stderr, "Unknown command: %s\n\n", command);
    umi_cli_print_help();
    return 2;
}
