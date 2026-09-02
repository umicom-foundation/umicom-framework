/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_dependencies.c
 *
 * PURPOSE:
 *   Inventory native dependency versions and, when OSV-Scanner is available,
 *   compare project manifests and C/C++ submodules with vulnerability data.
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

/*
 * Provide the dependency has flag operation used by this module and its client
 * applications.
 */
static int dependency_has_flag(int argc, char **argv, const char *option)
{
    int index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0; index < argc; ++index)
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(argv[index], option) == 0) return 1;
    return 0;
}

/*
 * Provide the dependency option value operation used by this module and its client
 * applications.
 */
static const char *dependency_option_value(
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

/* Provide the dependency audit operation used by this module and its client applications. */
static int dependency_audit(UmiCliContext *context, int argc, char **argv)
{
    const char *root = ".";
    const char *format_text = dependency_option_value(argc, argv, "--format");
    UmiCodeGuardDependencyAuditRequest request;
    UmiCodeGuardDependencyAuditReport report;
    UmiStatus status;
    int positional_seen = 0;
    int index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0; index < argc; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(argv[index], "--strict") == 0 ||
            strcmp(argv[index], "--dry-run") == 0) continue;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(argv[index], "--format") == 0) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (index + 1 >= argc) {
                (void)fprintf(stderr, "--format requires a value.\n");
                return 2;
            }
            ++index;
            continue;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (argv[index][0] == '-') {
            (void)fprintf(stderr, "Unknown dependency audit option: %s\n",
                          argv[index]);
            return 2;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (positional_seen) {
            (void)fprintf(stderr, "Only one project path may be provided.\n");
            return 2;
        }
        root = argv[index];
        positional_seen = 1;
    }
    umi_codeguard_dependency_audit_request_init(&request);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (format_text != NULL && !umi_codeguard_dependency_report_format_parse(
            format_text, &request.format)) {
        (void)fprintf(stderr, "Format must be vertical, json or sarif.\n");
        return 2;
    }
    status = umi_cli_context_prepare(context, root, 0, 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Unable to prepare dependency audit: %s\n",
                      umi_status_text(status));
        return 1;
    }
    request.project_root = context->project_root;
    request.environment = umi_environment_plan_variables(&context->environment);
    request.environment_count = context->environment.count;
    request.strict = dependency_has_flag(argc, argv, "--strict");
    request.dry_run = dependency_has_flag(argc, argv, "--dry-run");
    status = umi_codeguard_dependency_audit_execute(
        &context->discovery.profile, &request, &report);
    /* Apply this branch only when its contract condition is satisfied. */
    if (report.outcome == UMI_CODEGUARD_DEPENDENCY_PLANNED) {
        (void)printf(
            "Would discover OSV-Scanner and scan dependency manifests and "
            "C/C++ submodules recursively in %s.\n",
            context->project_root);
        return 0;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (report.outcome == UMI_CODEGUARD_DEPENDENCY_SKIPPED) {
        (void)fprintf(
            stderr,
            "SKIPPED: OSV-Scanner was not found, so no dependency CVE result "
            "is claimed. Install OSV-Scanner or use --strict to make this a "
            "required gate.\n");
        return status == UMI_STATUS_OK ? 0 : 1;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (report.output[0] != '\0') (void)fputs(report.output, stdout);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK ||
        report.outcome != UMI_CODEGUARD_DEPENDENCY_PASSED) {
        (void)fprintf(
            stderr,
            "Dependency vulnerability audit did not pass. Review the scanner "
            "output before building or publishing.\n");
        return 1;
    }
    (void)puts("Dependency vulnerability audit passed.");
    return 0;
}

/*
 * Provide the cli command dependencies operation used by this module and its client
 * applications.
 */
int umi_cli_command_dependencies(UmiCliContext *context, int argc, char **argv)
{
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (argc < 1 || strcmp(argv[0], "help") == 0 ||
        strcmp(argv[0], "--help") == 0 || strcmp(argv[0], "-h") == 0) {
        (void)puts(
            "Usage:\n"
            "  umicom dependencies check [umicom check options]\n"
            "  umicom dependencies inventory [PATH]\n"
            "  umicom dependencies audit [PATH] [--strict] [--dry-run] "
            "[--format vertical|json|sarif]\n\n"
            "The audit command never reports PASS when OSV-Scanner is absent.");
        return 0;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(argv[0], "check") == 0)
        return umi_cli_command_check(context, argc - 1, argv + 1);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(argv[0], "inventory") == 0) {
        char *check_arguments[3];
        int check_count = 1;
        check_arguments[0] = "--all";
        /* Apply this branch only when its contract condition is satisfied. */
        if (argc > 2) {
            (void)fprintf(stderr,
                          "Usage: umicom dependencies inventory [PATH]\n");
            return 2;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (argc == 2) {
            check_arguments[1] = "--project";
            check_arguments[2] = argv[1];
            check_count = 3;
        }
        return umi_cli_command_check(context, check_count, check_arguments);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(argv[0], "audit") == 0)
        return dependency_audit(context, argc - 1, argv + 1);
    (void)fprintf(stderr, "Unknown dependency command: %s\n", argv[0]);
    return 2;
}
