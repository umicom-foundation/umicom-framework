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
#include "source_reader.h"
#include "umicom/codeguard/source_naming.h"
#include "umicom/platform/directory.h"
#include "umicom/platform/path.h"

#include <stdio.h>
#include <string.h>

typedef struct ScanState {
    const UmiCodeGuardConfig *config;
    UmiCodeGuardDuplicateSet *duplicates;
    UmiCodeGuardResult *result;
    UmiStatus status;
    const UmiCancellationToken *cancellation;
} ScanState;


/* Keep language selection in one place for direct scanning and workspace
 * duplicate admission. Reuse the existing extension recogniser so language
 * flags cannot diverge between analysis phases. Filename policy is separate. */
static int umi_codeguard_scanner_source_selected(const UmiCodeGuardConfig *config,
    const char *path)
{
    if (!umi_codeguard_source_supported(path, config->profile.scan_cpp,
            config->profile.scan_headers)) return 0;
    return config->profile.scan_c ||
        !umi_codeguard_source_supported(path, 0, 0);
}

/* Preserve the first operational error. A cancellation request controls
 * whether further work or publication is allowed; it does not erase a cause
 * already reported by the filesystem or analysis engine. */
static UmiStatus umi_codeguard_scan_observe(UmiStatus status,
    const UmiCancellationToken *cancellation)
{
    return status == UMI_STATUS_OK &&
        umi_cancellation_token_is_requested(cancellation)
        ? UMI_STATUS_CANCELLED : status;
}

/* Compare directory components with Framework's existing platform path
 * semantics. Only components followed by a separator are directories:
 * a source filename containing "build" is not a generated directory. */
static int umi_codeguard_scanner_has_directory(const char *relative,
    const char *first, const char *second)
{
    const char *begin = relative;
    int previous_first = 0;
    while (*begin != '\0') {
        const char *end = begin;
        while (*end != '\0' && *end != '/' && *end != '\\') ++end;
        if (*end == '\0') break;
        char component[UMI_PATH_CAPACITY];
        size_t length = (size_t)(end - begin);
        /* relative comes from the bounded platform path service. */
        if (length >= sizeof component) return 0;
        memcpy(component, begin, length);
        component[length] = '\0';
        if (second == NULL && umi_path_equal(component, first)) return 1;
        if (second != NULL && previous_first && umi_path_equal(component, second))
            return 1;
        previous_first = umi_path_equal(component, first);
        begin = end + 1;
    }
    return 0;
}

/* Traversal supplies a path in the root's coordinate system. Compute the
 * policy path using the shared lexical service; keep the visited spelling
 * for reading files and recording findings. This is not a realpath sandbox. */
static UmiStatus umi_codeguard_scanner_relative_path(const char *root,
    const char *visited, char *relative, size_t capacity)
{
    UmiStatus status = umi_path_relative(root, visited, relative, capacity);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(relative, ".") == 0 || strcmp(relative, "..") == 0 ||
        (relative[0] == '.' && relative[1] == '.' &&
         (relative[2] == '/' || relative[2] == '\\')) ||
        umi_path_is_absolute(relative))
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/* Generated, third-party, and CodeGuard rule sources are excluded because
 * scanning those paths would report vendored code or the rule text itself. */
/* Apply exclusion policy only to workspace-relative directory components, so an excluded ancestor outside the selected root cannot suppress its source. The previous implementation is retained for engineering review. */
#if 0
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
#endif
/* Apply the generated-source option only to generated directories; repository metadata, vendor code and rule definitions remain excluded. Retain the unconditional policy for review. The previous implementation is retained for engineering review. */
#if 0
static int umi_codeguard_scanner_path_excluded(const char *relative)
{
    static const char *directories[] = {
        ".git", "build", "_CPack_Packages", "node_modules", "third_party"
    };
    for (size_t i = 0U; i < sizeof directories / sizeof directories[0]; ++i)
        if (umi_codeguard_scanner_has_directory(relative, directories[i], NULL))
            return 1;
    return umi_codeguard_scanner_has_directory(relative, "codeguard", "rules");
}
#endif
static int umi_codeguard_scanner_path_excluded(const char *relative,
    int scan_generated)
{
    static const char *directories[] = { ".git", "node_modules", "third_party" };
    for (size_t i = 0U; i < sizeof directories / sizeof directories[0]; ++i)
        if (umi_codeguard_scanner_has_directory(relative, directories[i], NULL))
            return 1;
    if (!scan_generated &&
        (umi_codeguard_scanner_has_directory(relative, "build", NULL) ||
         umi_codeguard_scanner_has_directory(relative, "_CPack_Packages", NULL)))
        return 1;
    return umi_codeguard_scanner_has_directory(relative, "codeguard", "rules");
}

/* A workspace template intentionally contains a self-contained Framework
 * payload. It is scanned for safety and naming, but it is not compared with
 * the live Framework tree as if it were a competing implementation. */
/* Classify template copies relative to the selected workspace, preserving ordinary scanning while omitting template payloads from duplicate comparisons. The previous implementation is retained for engineering review. */
#if 0
static int umi_codeguard_scanner_duplicate_candidate(const char *path)
{
    return strstr(path, "/templates/workspace/") == NULL &&
           strstr(path, "\\templates\\workspace\\") == NULL;
}
#endif
static int umi_codeguard_scanner_duplicate_candidate(const char *relative)
{
    return !umi_codeguard_scanner_has_directory(relative, "templates", "workspace");
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
/* Reuse shared cancellation at read and analysis boundaries while preserving ordinary file scanning. The previous implementation remains for review. */
#if 0
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
#endif
/* Use Framework native bounded input on every source pass, preserving original segment analysis and first-error behavior. The stdio implementation remains for review. The previous implementation remains for review. */
#if 0
UmiStatus umi_codeguard_scan_file_cancellable(const UmiCodeGuardConfig *config,
                                  const char *path,
                                  UmiCodeGuardResult *result,
                                  const UmiCancellationToken *cancellation)
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
    if (umi_cancellation_token_is_requested(cancellation))
        return UMI_STATUS_CANCELLED;
    /* Apply this operation only while the related capability or state is available. */
    /* The original extension gate ignored scan_c. Keep it for review;
     * both entry points now use the same complete language policy. */
#if 0
    if (!umi_codeguard_source_supported(path, config->profile.scan_cpp,
                                        config->profile.scan_headers)) {
        return UMI_STATUS_OK;
    }
#endif
    if (!umi_codeguard_scanner_source_selected(config, path)) return UMI_STATUS_OK;

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
    while (status == UMI_STATUS_OK) {
        status = umi_codeguard_scan_observe(status, cancellation);
        if (status != UMI_STATUS_OK) break;
        /* Initial cancellation-only read boundary retained for review. */
#if 0
        if (fgets(raw_line, sizeof(raw_line), file) == NULL) break;
        status = umi_codeguard_scan_observe(status, cancellation);
#endif
        if (fgets(raw_line, sizeof(raw_line), file) == NULL) break;
        /* A partial read can carry both bytes and an I/O error. Preserve that
         * cause before observing a concurrently requested cancellation. */
        if (ferror(file) != 0) { status = UMI_STATUS_IO_ERROR; break; }
        status = umi_codeguard_scan_observe(status, cancellation);
        if (status != UMI_STATUS_OK) break;
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
            /* Retain the void call for review; checked failure now prevents
             * a partial lifetime analysis from being published as complete. */
#if 0
            umi_codeguard_lifetime_scan(&lifetime, path, line_number,
                                        code_line, result);
#endif
            status = umi_codeguard_lifetime_scan_checked(&lifetime, path, line_number,
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
    status = umi_codeguard_scan_observe(status, cancellation);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && config->profile.scan_architecture) {
        status = umi_codeguard_architecture_scan_file(
            config->root, path, &config->profile, result);
    }
    return umi_codeguard_scan_observe(status, cancellation);
}
#endif
typedef struct UmiCodeGuardFileScan {
    const UmiCodeGuardConfig *config;
    const char *path;
    UmiCodeGuardResult *result;
    UmiCodeGuardRuleRegistry *registry;
    UmiCodeGuardLifetimeTracker lifetime;
    int inside_block_comment;
    const UmiCancellationToken *cancellation;
} UmiCodeGuardFileScan;

/* All analysis shares the reader's bounded native input. The callback borrows
 * one historical source segment; it cannot retain the reader's storage. */
static UmiStatus umi_codeguard_scanner_line(const char *raw, size_t size,
    size_t number, void *context)
{
    UmiCodeGuardFileScan *scan = context;
    UmiStatus status = umi_codeguard_scan_observe(UMI_STATUS_OK, scan->cancellation);
    if (status != UMI_STATUS_OK) return status;
    if (size > scan->config->profile.max_line_length)
        status = umi_codeguard_scanner_report_long_line(
            scan->config, scan->path, number, scan->result);
    if (status != UMI_STATUS_OK) return status;
    char code[8192];
    umi_codeguard_sanitise_code_line(raw, code, sizeof code, &scan->inside_block_comment);
    status = umi_codeguard_pattern_scan_line(scan->registry, scan->path,
        number, raw, code, scan->result);
    status = umi_codeguard_scan_observe(status, scan->cancellation);
    if (status == UMI_STATUS_OK)
        status = umi_codeguard_lifetime_scan_checked(&scan->lifetime,
            scan->path, number, code, scan->result);
    return umi_codeguard_scan_observe(status, scan->cancellation);
}

UmiStatus umi_codeguard_scan_file_cancellable(const UmiCodeGuardConfig *config,
    const char *path, UmiCodeGuardResult *result,
    const UmiCancellationToken *cancellation)
{
    if (config == NULL || path == NULL || result == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(cancellation)) return UMI_STATUS_CANCELLED;
    if (!umi_codeguard_scanner_source_selected(config, path)) return UMI_STATUS_OK;
    UmiCodeGuardFileScan scan = {.config = config, .path = path,
        .result = result, .cancellation = cancellation};
    UmiStatus status = umi_codeguard_rule_registry_create(&scan.registry);
    if (status == UMI_STATUS_OK)
        status = umi_codeguard_rule_registry_add_builtin(scan.registry);
    umi_codeguard_lifetime_init(&scan.lifetime);
    status = umi_codeguard_scan_observe(status, cancellation);
    if (status == UMI_STATUS_OK)
        status = umi_codeguard_source_visit_lines(path, config->profile.max_file_bytes,
            umi_codeguard_scanner_line, &scan, cancellation);
    umi_codeguard_rule_registry_destroy(scan.registry);
    status = umi_codeguard_scan_observe(status, cancellation);
    if (status == UMI_STATUS_OK && config->profile.scan_architecture)
        status = umi_codeguard_architecture_scan_file_cancellable(
            config->root, path, &config->profile, result, cancellation);
    return umi_codeguard_scan_observe(status, cancellation);
}

/* Keep the ordinary API on the same scanner, with no cancellation token. */
UmiStatus umi_codeguard_scan_file(const UmiCodeGuardConfig *config,
    const char *path, UmiCodeGuardResult *result)
{
    return umi_codeguard_scan_file_cancellable(config, path, result, NULL);
}

/* Apply repository-wide filename policy before limiting deeper analysis to C
 * and C++ sources. This also protects documentation and build configuration. */
/* Pass the borrowed token through per-file work and observe requests between independent analysis phases. The previous implementation is retained for engineering review. */
#if 0
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
#endif
/* Derive one workspace-relative policy path before exclusions while preserving visited paths for analysis, diagnostics and duplicate storage. The previous implementation is retained for engineering review. */
#if 0
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
    state->status = umi_codeguard_scan_observe(state->status, state->cancellation);
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
    state->status = umi_codeguard_scan_observe(state->status, state->cancellation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (state->status != UMI_STATUS_OK ||
        !umi_codeguard_source_supported(
            info->path, state->config->profile.scan_cpp,
            state->config->profile.scan_headers)) {
        return state->status;
    }

    state->status = umi_codeguard_scan_file_cancellable(state->config, info->path,
                                            state->result, state->cancellation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (state->status == UMI_STATUS_OK &&
        state->config->profile.scan_duplicates &&
        umi_codeguard_scanner_duplicate_candidate(info->path)) {
        state->status = umi_codeguard_duplicate_set_add(state->duplicates,
                                                        info->path);
    }
    state->status = umi_codeguard_scan_observe(state->status, state->cancellation);
    return state->status;
}
#endif
/* Use the same language policy before per-file analysis and duplicate collection, and pass the generated-source setting into workspace exclusions. The previous implementation is retained for engineering review. */
#if 0
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
    state->status = umi_codeguard_scan_observe(state->status, state->cancellation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (state->status != UMI_STATUS_OK) {
        return state->status;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (info->kind != UMI_FILE_KIND_REGULAR) return UMI_STATUS_OK;
    char relative[UMI_PATH_CAPACITY];
    state->status = umi_codeguard_scanner_relative_path(
        state->config->root, info->path, relative, sizeof relative);
    if (state->status != UMI_STATUS_OK) return state->status;
    if (umi_codeguard_scanner_path_excluded(relative)) return UMI_STATUS_OK;

    /* Apply this branch only when its contract condition is satisfied. */
    if (state->config->profile.scan_source_names) {
        state->status = umi_codeguard_source_name_audit(info->path,
                                                        state->result);
    }
    state->status = umi_codeguard_scan_observe(state->status, state->cancellation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (state->status != UMI_STATUS_OK ||
        !umi_codeguard_source_supported(
            info->path, state->config->profile.scan_cpp,
            state->config->profile.scan_headers)) {
        return state->status;
    }

    state->status = umi_codeguard_scan_file_cancellable(state->config, info->path,
                                            state->result, state->cancellation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (state->status == UMI_STATUS_OK &&
        state->config->profile.scan_duplicates &&
        umi_codeguard_scanner_duplicate_candidate(relative)) {
        state->status = umi_codeguard_duplicate_set_add(state->duplicates,
                                                        info->path);
    }
    state->status = umi_codeguard_scan_observe(state->status, state->cancellation);
    return state->status;
}
#endif
/* Apply the same file budget and cancellation token to duplicate fingerprint input, including files that change after analysis. The previous implementation is retained for engineering review. */
#if 0
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
    state->status = umi_codeguard_scan_observe(state->status, state->cancellation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (state->status != UMI_STATUS_OK) {
        return state->status;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (info->kind != UMI_FILE_KIND_REGULAR) return UMI_STATUS_OK;
    char relative[UMI_PATH_CAPACITY];
    state->status = umi_codeguard_scanner_relative_path(
        state->config->root, info->path, relative, sizeof relative);
    if (state->status != UMI_STATUS_OK) return state->status;
    if (umi_codeguard_scanner_path_excluded(relative, state->config->profile.scan_generated)) return UMI_STATUS_OK;

    /* Apply this branch only when its contract condition is satisfied. */
    if (state->config->profile.scan_source_names) {
        state->status = umi_codeguard_source_name_audit(info->path,
                                                        state->result);
    }
    state->status = umi_codeguard_scan_observe(state->status, state->cancellation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (state->status != UMI_STATUS_OK ||
        !umi_codeguard_scanner_source_selected(state->config, info->path)) {
        return state->status;
    }

    state->status = umi_codeguard_scan_file_cancellable(state->config, info->path,
                                            state->result, state->cancellation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (state->status == UMI_STATUS_OK &&
        state->config->profile.scan_duplicates &&
        umi_codeguard_scanner_duplicate_candidate(relative)) {
        state->status = umi_codeguard_duplicate_set_add(state->duplicates,
                                                        info->path);
    }
    state->status = umi_codeguard_scan_observe(state->status, state->cancellation);
    return state->status;
}
#endif
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
    state->status = umi_codeguard_scan_observe(state->status, state->cancellation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (state->status != UMI_STATUS_OK) {
        return state->status;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (info->kind != UMI_FILE_KIND_REGULAR) return UMI_STATUS_OK;
    char relative[UMI_PATH_CAPACITY];
    state->status = umi_codeguard_scanner_relative_path(
        state->config->root, info->path, relative, sizeof relative);
    if (state->status != UMI_STATUS_OK) return state->status;
    if (umi_codeguard_scanner_path_excluded(relative, state->config->profile.scan_generated)) return UMI_STATUS_OK;

    /* Apply this branch only when its contract condition is satisfied. */
    if (state->config->profile.scan_source_names) {
        state->status = umi_codeguard_source_name_audit(info->path,
                                                        state->result);
    }
    state->status = umi_codeguard_scan_observe(state->status, state->cancellation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (state->status != UMI_STATUS_OK ||
        !umi_codeguard_scanner_source_selected(state->config, info->path)) {
        return state->status;
    }

    state->status = umi_codeguard_scan_file_cancellable(state->config, info->path,
                                            state->result, state->cancellation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (state->status == UMI_STATUS_OK &&
        state->config->profile.scan_duplicates &&
        umi_codeguard_scanner_duplicate_candidate(relative)) {
        state->status = umi_codeguard_duplicate_set_add_bounded(state->duplicates,
            info->path, state->config->profile.max_file_bytes, state->cancellation);
    }
    state->status = umi_codeguard_scan_observe(state->status, state->cancellation);
    return state->status;
}

/* Provide the codeguard scan operation used by this module and its client applications. */
/* Use Framework cancellable traversal and preserve the existing scan entry point without duplicating scanning rules. The previous implementation remains for review. */
#if 0
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
#endif
UmiStatus umi_codeguard_scan_cancellable(const UmiCodeGuardConfig *config,
                             UmiCodeGuardResult *result,
                             const UmiCancellationToken *cancellation)
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

    if (umi_cancellation_token_is_requested(cancellation))
        return UMI_STATUS_CANCELLED;
    status = umi_codeguard_duplicate_set_create(&state.duplicates);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    state.config = config;
    state.result = result;
    state.status = UMI_STATUS_OK;
    state.cancellation = cancellation;
    options = umi_directory_walk_options_default();
    options.recursive = 1;
    options.include_files = 1;
    options.include_directories = 0;
    options.include_hidden = 0;
    options.follow_symbolic_links = 0;
    options.max_depth = 64U;

    status = UmiDirectoryWalkCancellable(config->root, &options,
                                umi_codeguard_scanner_visit, &state, cancellation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = state.status;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    status = umi_codeguard_scan_observe(status, cancellation);
    if (status == UMI_STATUS_OK && config->profile.scan_duplicates) {
        status = umi_codeguard_duplicate_emit(state.duplicates, result);
    }

    umi_codeguard_duplicate_set_destroy(state.duplicates);
    return umi_codeguard_scan_observe(status, cancellation);
}

/* Compatibility entry point: NULL token keeps ordinary synchronous scanning. */
UmiStatus umi_codeguard_scan(const UmiCodeGuardConfig *config,
    UmiCodeGuardResult *result)
{
    return umi_codeguard_scan_cancellable(config, result, NULL);
}
