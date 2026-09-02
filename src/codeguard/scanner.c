/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/scanner.c
 *
 * PURPOSE:
 *   Coordinate recursive source scanning, lifetime checks, architecture
 *   metrics and duplicate detection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */

#include "umicom/codeguard/scanner.h"
#include "umicom/codeguard/architecture.h"
#include "umicom/codeguard/duplicate.h"
#include "umicom/codeguard/lifetime.h"
#include "umicom/codeguard/pattern_scan.h"
#include "umicom/codeguard/rule_registry.h"
#include "umicom/codeguard/sanitise.h"
#include "umicom/codeguard/source.h"
#include "umicom/codeguard/source_naming.h"
#include "umicom/platform/directory.h"

#include <stdio.h>
#include <string.h>

typedef struct ScanState {
    const UmiCodeGuardConfig *config;
    UmiCodeGuardDuplicateSet *duplicates;
    UmiCodeGuardResult *result;
    UmiStatus status;
} ScanState;

/* Generated, third-party, and CodeGuard rule sources are excluded because
 * scanning those paths would report vendored code or the rule text itself. */
static int umi_codeguard_scanner_path_excluded(const char *path)
{
    static const char *excluded_fragments[] = {
        "/.git/", "\\.git\\",
        "/build/", "\\build\\",
        "/_CPack_Packages/", "\\_CPack_Packages\\",
        "/node_modules/", "\\node_modules\\",
        "/third_party/", "\\third_party\\",
        "/codeguard/rules/", "\\codeguard\\rules\\"
    };
    size_t index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < sizeof(excluded_fragments) / sizeof(excluded_fragments[0]);
         ++index) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (strstr(path, excluded_fragments[index]) != NULL) {
            return 1;
        }
    }
    return 0;
}

/* A workspace template intentionally contains a self-contained Framework
 * payload. It is scanned for safety and naming, but it is not compared with
 * the live Framework tree as if it were a competing implementation. */
static int umi_codeguard_scanner_duplicate_candidate(const char *path)
{
    return strstr(path, "/templates/workspace/") == NULL &&
           strstr(path, "\\templates\\workspace\\") == NULL;
}

/* Emit a small focused readability finding instead of mixing this policy into
 * the security pattern registry. */
static UmiStatus umi_codeguard_scanner_report_long_line(
    const UmiCodeGuardConfig *config,
    const char *path,
    size_t line,
    UmiCodeGuardResult *result)
{
    UmiCodeGuardFinding finding = {0};

    (void)snprintf(finding.rule_id, sizeof(finding.rule_id),
                   "CODEGUARD-QUALITY-LINE-001");
    finding.severity = UMI_CODEGUARD_LOW;
    finding.category = UMI_CODEGUARD_CATEGORY_QUALITY;
    finding.confidence = 100U;
    (void)snprintf(finding.path, sizeof(finding.path), "%s", path);
    finding.line = line;
    finding.column = config->profile.max_line_length + 1U;
    (void)snprintf(finding.message, sizeof(finding.message),
                   "Source line exceeds the configured readability limit.");
    (void)snprintf(
        finding.remediation, sizeof(finding.remediation),
        "Split the expression or statement so reviews and diagnostics remain "
        "readable.");
    return umi_codeguard_result_add(result, &finding);
}

/*
 * Provide the codeguard scan file operation used by this module and its client
 * applications.
 */
UmiStatus umi_codeguard_scan_file(const UmiCodeGuardConfig *config,
                                  const char *path,
                                  UmiCodeGuardResult *result)
{
    FILE *file;
    char raw_line[8192];
    char code_line[8192];
    size_t line_number = 0U;
    int inside_block_comment = 0;
    UmiCodeGuardRuleRegistry *registry = NULL;
    UmiCodeGuardLifetimeTracker lifetime;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (config == NULL || path == NULL || result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_codeguard_source_supported(path, config->profile.scan_cpp,
                                        config->profile.scan_headers)) {
        return UMI_STATUS_OK;
    }

    file = fopen(path, "rb");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (file == NULL) {
        return UMI_STATUS_IO_ERROR;
    }

    status = umi_codeguard_rule_registry_create(&registry);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_codeguard_rule_registry_add_builtin(registry);
    }
    umi_codeguard_lifetime_init(&lifetime);

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (status == UMI_STATUS_OK &&
           fgets(raw_line, sizeof(raw_line), file) != NULL) {
        ++line_number;
        /* Apply this branch only when its contract condition is satisfied. */
        if (strlen(raw_line) > config->profile.max_line_length) {
            status = umi_codeguard_scanner_report_long_line(
                config, path, line_number, result);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            break;
        }

        umi_codeguard_sanitise_code_line(raw_line, code_line,
                                         sizeof(code_line),
                                         &inside_block_comment);
        status = umi_codeguard_pattern_scan_line(
            registry, path, line_number, raw_line, code_line, result);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            umi_codeguard_lifetime_scan(&lifetime, path, line_number,
                                        code_line, result);
        }
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (ferror(file) != 0 && status == UMI_STATUS_OK) {
        status = UMI_STATUS_IO_ERROR;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (fclose(file) != 0 && status == UMI_STATUS_OK) {
        status = UMI_STATUS_IO_ERROR;
    }
    umi_codeguard_rule_registry_destroy(registry);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && config->profile.scan_architecture) {
        status = umi_codeguard_architecture_scan_file(
            config->root, path, &config->profile, result);
    }
    return status;
}

/* Apply repository-wide filename policy before limiting deeper analysis to C
 * and C++ sources. This also protects documentation and build configuration. */
static UmiStatus umi_codeguard_scanner_visit(const UmiFileInfo *info,
                                             void *user)
{
    ScanState *state = (ScanState *)user;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL || info == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (state->status != UMI_STATUS_OK) {
        return state->status;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (info->kind != UMI_FILE_KIND_REGULAR ||
        umi_codeguard_scanner_path_excluded(info->path)) {
        return UMI_STATUS_OK;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (state->config->profile.scan_source_names) {
        state->status = umi_codeguard_source_name_audit(info->path,
                                                        state->result);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (state->status != UMI_STATUS_OK ||
        !umi_codeguard_source_supported(
            info->path, state->config->profile.scan_cpp,
            state->config->profile.scan_headers)) {
        return state->status;
    }

    state->status = umi_codeguard_scan_file(state->config, info->path,
                                            state->result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (state->status == UMI_STATUS_OK &&
        state->config->profile.scan_duplicates &&
        umi_codeguard_scanner_duplicate_candidate(info->path)) {
        state->status = umi_codeguard_duplicate_set_add(state->duplicates,
                                                        info->path);
    }
    return state->status;
}

/* Provide the codeguard scan operation used by this module and its client applications. */
UmiStatus umi_codeguard_scan(const UmiCodeGuardConfig *config,
                             UmiCodeGuardResult *result)
{
    UmiDirectoryWalkOptions options;
    ScanState state = {0};
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (config == NULL || config->root == NULL || result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_codeguard_duplicate_set_create(&state.duplicates);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    state.config = config;
    state.result = result;
    state.status = UMI_STATUS_OK;
    options = umi_directory_walk_options_default();
    options.recursive = 1;
    options.include_files = 1;
    options.include_directories = 0;
    options.include_hidden = 0;
    options.follow_symbolic_links = 0;
    options.max_depth = 64U;

    status = umi_directory_walk(config->root, &options,
                                umi_codeguard_scanner_visit, &state);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = state.status;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && config->profile.scan_duplicates) {
        status = umi_codeguard_duplicate_emit(state.duplicates, result);
    }

    umi_codeguard_duplicate_set_destroy(state.duplicates);
    return status;
}
