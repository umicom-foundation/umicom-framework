/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_security.c
 *
 * PURPOSE:
 *   Provide native quality, security, architecture and memory-risk scans using
 *   Framework-owned CodeGuard profiles and reports.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "cli.h"

#include "umicom/codeguard/codeguard.h"

#include <stdio.h>
#include <string.h>

typedef enum UmiCliScanSelection {
    UMI_CLI_SCAN_ALL = 0,
    UMI_CLI_SCAN_MEMORY = 1
} UmiCliScanSelection;

/* Provide the scan option value operation used by this module and its client applications. */
static const char *scan_option_value(int argc, char **argv, const char *option)
{
    int index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0; index + 1 < argc; ++index)
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(argv[index], option) == 0) return argv[index + 1];
    return NULL;
}

/* Provide the scan has flag operation used by this module and its client applications. */
static int scan_has_flag(int argc, char **argv, const char *option)
{
    int index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0; index < argc; ++index)
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(argv[index], option) == 0) return 1;
    return 0;
}

/*
 * Provide the scan report format operation used by this module and its client
 * applications.
 */
static UmiCodeGuardReportFormat scan_report_format(const char *text, int *valid)
{
    *valid = 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || strcmp(text, "text") == 0)
        return UMI_CODEGUARD_REPORT_TEXT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(text, "json") == 0) return UMI_CODEGUARD_REPORT_JSON;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(text, "sarif") == 0) return UMI_CODEGUARD_REPORT_SARIF;
    *valid = 0;
    return UMI_CODEGUARD_REPORT_TEXT;
}

/* Provide the scan profile operation used by this module and its client applications. */
static UmiCodeGuardProfile scan_profile(const char *text, int *valid)
{
    *valid = 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || strcmp(text, "default") == 0)
        return umi_codeguard_profile_default();
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(text, "security") == 0)
        return umi_codeguard_profile_security();
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(text, "architecture") == 0)
        return umi_codeguard_profile_architecture();
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(text, "ci") == 0 || strcmp(text, "quality") == 0)
        return umi_codeguard_profile_ci();
    *valid = 0;
    return umi_codeguard_profile_default();
}

/* Provide the memory category operation used by this module and its client applications. */
static int memory_category(UmiCodeGuardCategory category)
{
    return category == UMI_CODEGUARD_CATEGORY_MEMORY ||
           category == UMI_CODEGUARD_CATEGORY_BUFFER ||
           category == UMI_CODEGUARD_CATEGORY_RESOURCE ||
           category == UMI_CODEGUARD_CATEGORY_STRING;
}

/* Provide the select findings operation used by this module and its client applications. */
static UmiStatus select_findings(const UmiCodeGuardResult *source,
                                 UmiCliScanSelection selection,
                                 UmiCodeGuardResult *destination)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_codeguard_result_count(source); ++index) {
        const UmiCodeGuardFinding *finding =
            umi_codeguard_result_at(source, index);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (finding != NULL &&
            (selection == UMI_CLI_SCAN_ALL || memory_category(finding->category))) {
            UmiStatus status = umi_codeguard_result_add(destination, finding);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Perform scan through the module contract so client applications do not duplicate its
 * policy.
 */
static int scan_run(int argc,
                    char **argv,
                    const char *forced_profile,
                    UmiCliScanSelection selection)
{
    const char *root = ".";
    const char *profile_text = forced_profile;
    const char *format_text;
    const char *output_path;
    UmiCodeGuardResult *all_findings = NULL;
    UmiCodeGuardResult *selected_findings = NULL;
    UmiCodeGuardConfig config;
    UmiCodeGuardProfile profile;
    UmiCodeGuardReportFormat format;
    UmiCodeGuardSummary summary;
    UmiStatus status;
    int valid;
    int index;
    int positional_seen = 0;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (argc > 0 && (strcmp(argv[0], "scan") == 0 ||
                     strcmp(argv[0], "check") == 0)) {
        --argc;
        ++argv;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0; index < argc; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(argv[index], "--profile") == 0 ||
            strcmp(argv[index], "--format") == 0 ||
            strcmp(argv[index], "--output") == 0) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (index + 1 >= argc || argv[index + 1][0] == '\0' ||
                argv[index + 1][0] == '-') {
                (void)fprintf(stderr, "%s requires a value.\n", argv[index]);
                return 2;
            }
            ++index;
            continue;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(argv[index], "--summary") == 0) continue;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(argv[index], "--help") == 0 ||
            strcmp(argv[index], "-h") == 0) {
            (void)puts(
                "Usage: umicom quality scan [PATH] [--profile NAME] "
                "[--format text|json|sarif] [--output FILE] [--summary]\n"
                "Profiles: default, quality, security, architecture, ci");
            return 0;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (argv[index][0] == '-') {
            (void)fprintf(stderr, "Unknown scan option: %s\n", argv[index]);
            return 2;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (positional_seen) {
            (void)fprintf(stderr, "Only one scan path may be provided.\n");
            return 2;
        }
        root = argv[index];
        positional_seen = 1;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (forced_profile == NULL)
        profile_text = scan_option_value(argc, argv, "--profile");
    format_text = scan_option_value(argc, argv, "--format");
    output_path = scan_option_value(argc, argv, "--output");
    profile = scan_profile(profile_text, &valid);
    /* Apply this operation only while the related capability or state is available. */
    if (!valid) {
        (void)fprintf(stderr, "Unknown CodeGuard profile: %s\n", profile_text);
        return 2;
    }
    format = scan_report_format(format_text, &valid);
    /* Apply this operation only while the related capability or state is available. */
    if (!valid) {
        (void)fprintf(stderr, "Unknown report format: %s\n", format_text);
        return 2;
    }

    status = umi_codeguard_result_create(512U, &all_findings);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_codeguard_result_create(128U, &selected_findings);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        config = umi_codeguard_config_default(root);
        config.profile = profile;
        status = umi_codeguard_scan(&config, all_findings);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = select_findings(all_findings, selection, selected_findings);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "CodeGuard scan failed: %s\n",
                      umi_status_text(status));
        umi_codeguard_result_destroy(selected_findings);
        umi_codeguard_result_destroy(all_findings);
        return 1;
    }

    summary = umi_codeguard_summary_build(selected_findings);
    /* Apply this branch only when its contract condition is satisfied. */
    if (scan_has_flag(argc, argv, "--summary")) {
        (void)printf(
            "Umicom CodeGuard: %zu findings (critical=%zu high=%zu "
            "medium=%zu low=%zu info=%zu)\n",
            summary.total, summary.critical, summary.high, summary.medium,
            summary.low, summary.info);
    } else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (output_path != NULL) {
        status = umi_codeguard_report_file(output_path, format, selected_findings);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK)
            (void)printf("CodeGuard report: %s\n", output_path);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        status = umi_codeguard_report_write(stdout, format, selected_findings);
    }

    umi_codeguard_result_destroy(selected_findings);
    umi_codeguard_result_destroy(all_findings);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return 1;
    return umi_codeguard_summary_failed(&summary, profile.fail_on) ? 1 : 0;
}

/*
 * Provide the cli command quality operation used by this module and its client
 * applications.
 */
int umi_cli_command_quality(UmiCliContext *context, int argc, char **argv)
{
    (void)context;
    return scan_run(argc, argv, NULL, UMI_CLI_SCAN_ALL);
}

/*
 * Provide the cli command security operation used by this module and its client
 * applications.
 */
int umi_cli_command_security(UmiCliContext *context, int argc, char **argv)
{
    (void)context;
    return scan_run(argc, argv, "security", UMI_CLI_SCAN_ALL);
}

/*
 * Provide the cli command architecture operation used by this module and its client
 * applications.
 */
int umi_cli_command_architecture(UmiCliContext *context, int argc, char **argv)
{
    (void)context;
    return scan_run(argc, argv, "architecture", UMI_CLI_SCAN_ALL);
}

/*
 * Provide the cli command memory operation used by this module and its client
 * applications.
 */
int umi_cli_command_memory(UmiCliContext *context, int argc, char **argv)
{
    (void)context;
    return scan_run(argc, argv, "ci", UMI_CLI_SCAN_MEMORY);
}
