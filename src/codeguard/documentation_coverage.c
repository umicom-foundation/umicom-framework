/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/documentation_coverage.c
 *
 * PURPOSE:
 *   Implement non-mutating documentation coverage checks for C source and
 *   public header files while keeping the result useful to absolute beginners.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/codeguard/documentation_coverage.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* Convert a count ratio to an integer percentage without floating-point state. */
static unsigned coverage_percent(size_t documented, size_t total)
{
    size_t accumulator = 0U;
    unsigned percent = 0U;
    unsigned step;
    /* A file with no candidate of this kind has nothing missing in that category. */
    if (total == 0U) {
        return 100U;
    }
    /* One hundred bounded additions avoid documented * 100 integer overflow. */
    for (step = 0U; step < 100U; ++step) {
        /* Subtraction before addition keeps the accumulator below total. */
        if (accumulator >= total - documented) {
            accumulator -= total - documented;
            percent += 1U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            accumulator += documented;
        }
    }
    return percent;
}

/* Skip indentation so all following tests see the first meaningful character. */
static const char *skip_space(const char *text)
{
    /* Cast through unsigned char because ctype functions reject negative char values. */
    while (text != NULL && *text != '\0' &&
           isspace((unsigned char)*text) != 0) {
        ++text;
    }
    return text;
}

/* Recognise control statements separately so they are not counted as functions. */
static bool is_decision_line(const char *line)
{
    const char *text = skip_space(line);
    /* Requiring a following space or parenthesis avoids matching ordinary words. */
    return text != NULL &&
        ((strncmp(text, "if", 2U) == 0 &&
          (text[2] == '(' || isspace((unsigned char)text[2]) != 0)) ||
         (strncmp(text, "else if", 7U) == 0 &&
          (text[7] == '(' || isspace((unsigned char)text[7]) != 0)) ||
         (strncmp(text, "for", 3U) == 0 &&
          (text[3] == '(' || isspace((unsigned char)text[3]) != 0)) ||
         (strncmp(text, "while", 5U) == 0 &&
          (text[5] == '(' || isspace((unsigned char)text[5]) != 0)) ||
         (strncmp(text, "switch", 6U) == 0 &&
          (text[6] == '(' || isspace((unsigned char)text[6]) != 0)));
}

/* Identify ordinary function declarations and definitions without parsing C types. */
static bool is_function_line(const char *line)
{
    const char *text;
    const char *open;
    const char *name_start;
    const char *cursor;
    /* Generated macros and control flow are excluded because their comments follow other rules. */
    if (line == NULL || is_decision_line(line)) {
        return false;
    }
    text = skip_space(line);
    /* Indented preprocessor directives are not ordinary C function declarations. */
    if (text == NULL || text[0] == '#') {
        return false;
    }
    open = strchr(line, '(');
    /* A missing argument list cannot be a function declaration on this line. */
    if (open == NULL || open <= text) {
        return false;
    }
    cursor = open;
    /* Walk backwards over spaces between the function name and opening parenthesis. */
    while (cursor > text && isspace((unsigned char)cursor[-1]) != 0) {
        --cursor;
    }
    /* Walk backwards over the C identifier that names the function. */
    name_start = cursor;
    /* Identifier characters reveal the complete candidate function name. */
    while (name_start > text &&
           (isalnum((unsigned char)name_start[-1]) != 0 ||
            name_start[-1] == '_')) {
        --name_start;
    }
    /* Calls start with the name, while declarations have a return type before it. */
    if (name_start == text || name_start == cursor) {
        return false;
    }
    /* An assignment before the name identifies a call used as an expression. */
    if (memchr(text, '=', (size_t)(name_start - text)) != NULL) {
        return false;
    }
    /* Return statements call a function but do not declare one. */
    if (strncmp(text, "return", 6U) == 0 &&
        isspace((unsigned char)text[6]) != 0) {
        return false;
    }
    return true;
}

/* Detect comments on the current line and track multi-line comment state. */
static bool line_has_comment(const char *line, bool *inside_block_comment)
{
    const char *block_start;
    const char *block_end;
    const char *line_start;
    bool has_comment;
    /* Missing caller state would make block-comment tracking unreliable. */
    if (line == NULL || inside_block_comment == NULL) {
        return false;
    }
    has_comment = *inside_block_comment;
    block_start = strstr(line, "/*");
    block_end = strstr(line, "*/");
    line_start = strstr(line, "//");
    /* Any comment marker makes this line explanatory content for adjacency checks. */
    if (block_start != NULL || block_end != NULL || line_start != NULL) {
        has_comment = true;
    }
    /* A start without a later end carries comment state to following lines. */
    if (block_start != NULL &&
        (block_end == NULL || block_end < block_start)) {
        *inside_block_comment = true;
    }
    /* A closing marker ends block state after this line has been counted. */
    if (block_end != NULL) {
        *inside_block_comment = false;
    }
    return has_comment;
}

/* Decide whether a nearby comment explains the candidate instead of being remote prose. */
static bool has_adjacent_guidance(
    size_t current_line,
    size_t last_comment_line,
    bool current_line_has_comment)
{
    /* Same-line comments and comments within two preceding lines are considered adjacent. */
    if (current_line_has_comment) {
        return true;
    }
    return last_comment_line != 0U && current_line > last_comment_line &&
           current_line - last_comment_line <= 2U;
}

/* Create the gradual default required by new code without making legacy scans unusable. */
void umi_codeguard_documentation_policy_default(
    UmiCodeGuardDocumentationPolicy *out_policy)
{
    /* A NULL output is ignored so diagnostic setup remains safe during error handling. */
    if (out_policy == NULL) {
        return;
    }
    (void)memset(out_policy, 0, sizeof(*out_policy));
    out_policy->structure_size = (uint32_t)sizeof(*out_policy);
    out_policy->minimum_function_percent = 100U;
    out_policy->minimum_decision_percent = 80U;
    out_policy->require_standard_file_header = true;
}

/* Apply one policy to stable counts, suitable for release gates and Studio views. */
bool umi_codeguard_documentation_report_passes(
    const UmiCodeGuardDocumentationReport *report,
    const UmiCodeGuardDocumentationPolicy *policy)
{
    /* Invalid or older structures fail closed rather than hiding missing evidence. */
    if (report == NULL || policy == NULL ||
        report->structure_size < sizeof(*report) ||
        policy->structure_size < sizeof(*policy) ||
        policy->minimum_function_percent > 100U ||
        policy->minimum_decision_percent > 100U) {
        return false;
    }
    /* Header enforcement is optional for generated or imported compatibility sources. */
    if (policy->require_standard_file_header &&
        !report->has_standard_file_header) {
        return false;
    }
    return report->function_percent >= policy->minimum_function_percent &&
           report->decision_percent >= policy->minimum_decision_percent;
}

/* Read a single file and produce evidence without inserting or rewriting comments. */
UmiStatus umi_codeguard_documentation_scan_file(
    const char *path,
    const UmiCodeGuardDocumentationPolicy *policy,
    UmiCodeGuardDocumentationReport *out_report)
{
    FILE *source;
    char line[8192];
    size_t last_comment_line = 0U;
    bool inside_block_comment = false;
    bool saw_purpose = false;
    bool saw_author = false;
    bool saw_licence = false;
    bool read_failed;
    int close_status;
    int written;
    /* Validate policy bounds before opening a caller-selected filesystem path. */
    if (path == NULL || path[0] == '\0' || policy == NULL ||
        out_report == NULL || policy->structure_size < sizeof(*policy) ||
        policy->minimum_function_percent > 100U ||
        policy->minimum_decision_percent > 100U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    source = fopen(path, "rb");
    /* An unreadable file produces a distinct I/O result instead of empty coverage. */
    if (source == NULL) {
        return UMI_STATUS_IO_ERROR;
    }
    (void)memset(out_report, 0, sizeof(*out_report));
    out_report->structure_size = (uint32_t)sizeof(*out_report);
    written = snprintf(out_report->path, sizeof(out_report->path), "%s", path);
    /* Truncated paths cannot identify reliable review evidence. */
    if (written < 0 || (size_t)written >= sizeof(out_report->path)) {
        (void)fclose(source);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Each physical line updates header evidence and nearby guidance state. */
    while (fgets(line, sizeof(line), source) != NULL) {
        bool has_comment;
        out_report->line_count += 1U;
        has_comment = line_has_comment(line, &inside_block_comment);
        /* The standard header is deliberately recognised only near the file beginning. */
        if (out_report->line_count <= 40U) {
            /* PURPOSE states why the file exists. */
            if (strstr(line, "PURPOSE:") != NULL) {
                saw_purpose = true;
            }
            /* AUTHOR AND ORGANISATION records project stewardship. */
            if (strstr(line, "AUTHOR AND ORGANISATION:") != NULL) {
                saw_author = true;
            }
            /* LICENCE records the permitted use of the source. */
            if (strstr(line, "LICENCE:") != NULL) {
                saw_licence = true;
            }
        }
        /* Functions and decisions use the comment state from before or on their line. */
        if (is_function_line(line)) {
            out_report->function_count += 1U;
            /* Nearby guidance explains the function to a future reader. */
            if (has_adjacent_guidance(out_report->line_count,
                                      last_comment_line, has_comment)) {
                out_report->documented_function_count += 1U;
            }
        }
        /* Count control-flow candidates independently from function candidates. */
        if (is_decision_line(line)) {
            out_report->decision_count += 1U;
            /* Nearby guidance explains why the branch or repetition exists. */
            if (has_adjacent_guidance(out_report->line_count,
                                      last_comment_line, has_comment)) {
                out_report->explained_decision_count += 1U;
            }
        }
        /* Record the latest explanatory line after evaluating current candidates. */
        if (has_comment) {
            last_comment_line = out_report->line_count;
        }
    }
    /* Capture read state before closing, then always release the operating-system handle. */
    read_failed = ferror(source) != 0;
    close_status = fclose(source);
    /* Either failure makes the evidence incomplete and unsuitable for a quality gate. */
    if (read_failed || close_status != 0) {
        return UMI_STATUS_IO_ERROR;
    }
    out_report->has_standard_file_header = saw_purpose && saw_author && saw_licence;
    out_report->function_percent = coverage_percent(
        out_report->documented_function_count, out_report->function_count);
    out_report->decision_percent = coverage_percent(
        out_report->explained_decision_count, out_report->decision_count);
    out_report->passes = umi_codeguard_documentation_report_passes(
        out_report, policy);
    return UMI_STATUS_OK;
}
