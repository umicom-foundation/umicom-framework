/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/diagnostic_parser_support.c
 *
 * PURPOSE:
 *   Implement common bounded diagnostic parsing for native/polyglot tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/diagnostic_parser_support.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) source = "";

    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

/* Provide the find case folded operation used by this module and its client applications. */
static const char *find_case_folded(const char *text, const char *needle)
{
    const char *start;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || needle == NULL || needle[0] == '\0') return text;

    /* Visit each bounded item once so every record receives the same rule. */
    for (start = text; *start != '\0'; ++start) {
        const char *a = start;
        const char *b = needle;

        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (*a != '\0' && *b != '\0' &&
               tolower((unsigned char)*a) ==
                   tolower((unsigned char)*b)) {
            ++a;
            ++b;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (*b == '\0') return start;
    }

    return NULL;
}

/*
 * Provide the developer diagnostic severity from text operation used by this module and
 * its client applications.
 */
UmiDeveloperProductivitySeverity
umi_developer_diagnostic_severity_from_text(const char *text)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (find_case_folded(text, "fatal") != NULL) {
        return UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_FATAL;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (find_case_folded(text, "error") != NULL ||
        find_case_folded(text, "failed") != NULL ||
        find_case_folded(text, "undefined reference") != NULL) {
        return UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_ERROR;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (find_case_folded(text, "warning") != NULL) {
        return UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_WARNING;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (find_case_folded(text, "note") != NULL ||
        find_case_folded(text, "info") != NULL) {
        return UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_INFO;
    }

    return UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_INFO;
}

/* Provide the init problem operation used by this module and its client applications. */
static void init_problem(UmiDeveloperProblem *problem,
                         const char *source_id,
                         const char *line)
{
    (void)memset(problem, 0, sizeof(*problem));
    copy_text(problem->source, sizeof(problem->source), source_id);
    copy_text(problem->message, sizeof(problem->message), line);
    problem->severity = umi_developer_diagnostic_severity_from_text(line);
    problem->transient = 1;
    problem->revision = 1U;
}

/*
 * Provide the parse colon location operation used by this module and its client
 * applications.
 */
static UmiStatus parse_colon_location(
    const char *source_id,
    const char *line,
    UmiDeveloperProblem *problem,
    int *out_matched)
{
    char path[UMI_DEVELOPER_PRODUCTIVITY_PATH_CAPACITY];
    char severity[64];
    char message[UMI_DEVELOPER_PRODUCTIVITY_TEXT_CAPACITY];
    unsigned long line_number = 0UL;
    unsigned long column_number = 0UL;
    int parsed;

    /*
     * Windows drive-letter paths contain a colon before the line/column
     * separators. Handle that prefix explicitly before the portable form.
     */
    if (isalpha((unsigned char)line[0]) && line[1] == ':') {
        char drive = '\0';
        char tail[UMI_DEVELOPER_PRODUCTIVITY_PATH_CAPACITY - 2U];

        parsed = sscanf(
            line,
            "%c:%2045[^:]:%lu:%lu: %63[^:]: %1023[^\n]",
            &drive,
            tail,
            &line_number,
            &column_number,
            severity,
            message);

        /* Apply this branch only when its contract condition is satisfied. */
        if (parsed == 6) {
            (void)snprintf(path, sizeof(path), "%c:%s", drive, tail);
            parsed = 5;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            parsed = sscanf(
                line,
                "%c:%2045[^:]:%lu: %63[^:]: %1023[^\n]",
                &drive,
                tail,
                &line_number,
                severity,
                message);
            column_number = 0UL;

            /* Apply this branch only when its contract condition is satisfied. */
            if (parsed == 5) {
                (void)snprintf(path, sizeof(path), "%c:%s", drive, tail);
                parsed = 4;
            }
        }
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        parsed = sscanf(
            line,
            "%2047[^:]:%lu:%lu: %63[^:]: %1023[^\n]",
            path,
            &line_number,
            &column_number,
            severity,
            message);

        /* Apply this branch only when its contract condition is satisfied. */
        if (parsed != 5) {
            parsed = sscanf(
                line,
                "%2047[^:]:%lu: %63[^:]: %1023[^\n]",
                path,
                &line_number,
                severity,
                message);
            column_number = 0UL;
        }
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (parsed != 5 && parsed != 4) {
        *out_matched = 0;
        return UMI_STATUS_OK;
    }

    init_problem(problem, source_id, line);
    copy_text(problem->location.uri, sizeof(problem->location.uri), path);
    problem->location.line = (size_t)line_number;
    problem->location.column = (size_t)column_number;
    copy_text(problem->message, sizeof(problem->message), message);
    problem->severity =
        umi_developer_diagnostic_severity_from_text(severity);
    *out_matched = 1;
    return UMI_STATUS_OK;
}

/*
 * Provide the parse paren location operation used by this module and its client
 * applications.
 */
static UmiStatus parse_paren_location(
    const char *source_id,
    const char *line,
    UmiDeveloperProblem *problem,
    int *out_matched)
{
    char path[UMI_DEVELOPER_PRODUCTIVITY_PATH_CAPACITY];
    char severity[64];
    char code[UMI_DEVELOPER_PRODUCTIVITY_ID_CAPACITY];
    char message[UMI_DEVELOPER_PRODUCTIVITY_TEXT_CAPACITY];
    unsigned long line_number = 0UL;
    unsigned long column_number = 0UL;
    int parsed = sscanf(
        line,
        "%2047[^'(](%lu,%lu): %63s %191[^:]: %1023[^\n]",
        path,
        &line_number,
        &column_number,
        severity,
        code,
        message);

    /* Apply this branch only when its contract condition is satisfied. */
    if (parsed < 5) {
        parsed = sscanf(
            line,
            "%2047[^'(](%lu): %63s %191[^:]: %1023[^\n]",
            path,
            &line_number,
            severity,
            code,
            message);
        column_number = 0UL;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (parsed < 5) {
        *out_matched = 0;
        return UMI_STATUS_OK;
    }

    init_problem(problem, source_id, line);
    copy_text(problem->location.uri, sizeof(problem->location.uri), path);
    problem->location.line = (size_t)line_number;
    problem->location.column = (size_t)column_number;
    copy_text(problem->code, sizeof(problem->code), code);
    copy_text(problem->message, sizeof(problem->message), message);
    problem->severity =
        umi_developer_diagnostic_severity_from_text(severity);
    *out_matched = 1;
    return UMI_STATUS_OK;
}

/* Provide the parse cmake operation used by this module and its client applications. */
static UmiStatus parse_cmake(
    const char *source_id,
    const char *line,
    UmiDeveloperProblem *problem,
    int *out_matched)
{
    char severity[64];
    char path[UMI_DEVELOPER_PRODUCTIVITY_PATH_CAPACITY];
    unsigned long line_number = 0UL;
    int parsed;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strncmp(line, "CMake ", 6U) != 0) {
        *out_matched = 0;
        return UMI_STATUS_OK;
    }

    parsed = sscanf(
        line,
        "CMake %63s at %2047[^:]:%lu",
        severity,
        path,
        &line_number);

    init_problem(problem, source_id, line);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (parsed >= 3) {
        copy_text(problem->location.uri, sizeof(problem->location.uri), path);
        problem->location.line = (size_t)line_number;
        problem->severity =
            umi_developer_diagnostic_severity_from_text(severity);
    }

    *out_matched = 1;
    return UMI_STATUS_OK;
}

/* Provide the parse prefix operation used by this module and its client applications. */
static UmiStatus parse_prefix(
    const char *source_id,
    const char *required_token,
    const char *line,
    UmiDeveloperProblem *problem,
    int *out_matched)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (required_token != NULL &&
        find_case_folded(line, required_token) == NULL) {
        *out_matched = 0;
        return UMI_STATUS_OK;
    }

    init_problem(problem, source_id, line);
    *out_matched = 1;
    return UMI_STATUS_OK;
}

/*
 * Provide the parse python trace operation used by this module and its client
 * applications.
 */
static UmiStatus parse_python_trace(
    const char *source_id,
    const char *line,
    UmiDeveloperProblem *problem,
    int *out_matched)
{
    char path[UMI_DEVELOPER_PRODUCTIVITY_PATH_CAPACITY];
    unsigned long line_number = 0UL;
    int parsed = sscanf(
        line,
        " File \"%2047[^\"]\", line %lu",
        path,
        &line_number);

    /* Apply this branch only when its contract condition is satisfied. */
    if (parsed != 2) {
        parsed = sscanf(
            line,
            "  File \"%2047[^\"]\", line %lu",
            path,
            &line_number);
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (parsed != 2) {
        *out_matched = 0;
        return UMI_STATUS_OK;
    }

    init_problem(problem, source_id, line);
    copy_text(problem->location.uri, sizeof(problem->location.uri), path);
    problem->location.line = (size_t)line_number;
    copy_text(
        problem->message,
        sizeof(problem->message),
        "Python traceback location");
    problem->severity = UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_ERROR;
    *out_matched = 1;
    return UMI_STATUS_OK;
}

/* Provide the parse typescript operation used by this module and its client applications. */
static UmiStatus parse_typescript(
    const char *source_id,
    const char *line,
    UmiDeveloperProblem *problem,
    int *out_matched)
{
    char path[UMI_DEVELOPER_PRODUCTIVITY_PATH_CAPACITY];
    char severity[64];
    char code[UMI_DEVELOPER_PRODUCTIVITY_ID_CAPACITY];
    char message[UMI_DEVELOPER_PRODUCTIVITY_TEXT_CAPACITY];
    unsigned long line_number = 0UL;
    unsigned long column_number = 0UL;
    int parsed = sscanf(
        line,
        "%2047[^(](%lu,%lu): %63s %191[^:]: %1023[^\n]",
        path,
        &line_number,
        &column_number,
        severity,
        code,
        message);

    /* Apply this branch only when its contract condition is satisfied. */
    if (parsed != 6) {
        *out_matched = 0;
        return UMI_STATUS_OK;
    }

    init_problem(problem, source_id, line);
    copy_text(problem->location.uri, sizeof(problem->location.uri), path);
    problem->location.line = (size_t)line_number;
    problem->location.column = (size_t)column_number;
    copy_text(problem->code, sizeof(problem->code), code);
    copy_text(problem->message, sizeof(problem->message), message);
    problem->severity =
        umi_developer_diagnostic_severity_from_text(severity);
    *out_matched = 1;
    return UMI_STATUS_OK;
}

/*
 * Provide the developer diagnostic parse pattern operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_diagnostic_parse_pattern(
    UmiDeveloperDiagnosticPattern pattern,
    const char *source_id,
    const char *required_token,
    const char *line,
    UmiDeveloperProblem *out_problem,
    int *out_matched)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source_id == NULL || line == NULL ||
        out_problem == NULL || out_matched == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_matched = 0;

    /* Select the behaviour associated with the requested command or state value. */
    switch (pattern) {
        case UMI_DEVELOPER_DIAGNOSTIC_COLON_LOCATION:
            return parse_colon_location(
                source_id, line, out_problem, out_matched);
        case UMI_DEVELOPER_DIAGNOSTIC_PAREN_LOCATION:
            return parse_paren_location(
                source_id, line, out_problem, out_matched);
        case UMI_DEVELOPER_DIAGNOSTIC_CMAKE:
            return parse_cmake(
                source_id, line, out_problem, out_matched);
        case UMI_DEVELOPER_DIAGNOSTIC_PREFIX:
            return parse_prefix(
                source_id,
                required_token,
                line,
                out_problem,
                out_matched);
        case UMI_DEVELOPER_DIAGNOSTIC_PYTHON_TRACE:
            return parse_python_trace(
                source_id, line, out_problem, out_matched);
        case UMI_DEVELOPER_DIAGNOSTIC_TYPESCRIPT:
            return parse_typescript(
                source_id, line, out_problem, out_matched);
        default:
            return UMI_STATUS_INVALID_ARGUMENT;
    }
}
