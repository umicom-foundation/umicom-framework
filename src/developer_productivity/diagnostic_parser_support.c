/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/diagnostic_parser_support.c
 *
 * PURPOSE:
 *   Implement common bounded diagnostic parsing for native/polyglot tools.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/diagnostic_parser_support.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";

    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

static const char *find_case_folded(const char *text, const char *needle)
{
    const char *start;

    if (text == NULL || needle == NULL || needle[0] == '\0') return text;

    for (start = text; *start != '\0'; ++start) {
        const char *a = start;
        const char *b = needle;

        while (*a != '\0' && *b != '\0' &&
               tolower((unsigned char)*a) ==
                   tolower((unsigned char)*b)) {
            ++a;
            ++b;
        }

        if (*b == '\0') return start;
    }

    return NULL;
}

UmiDeveloperProductivitySeverity
umi_developer_diagnostic_severity_from_text(const char *text)
{
    if (find_case_folded(text, "fatal") != NULL) {
        return UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_FATAL;
    }
    if (find_case_folded(text, "error") != NULL ||
        find_case_folded(text, "failed") != NULL ||
        find_case_folded(text, "undefined reference") != NULL) {
        return UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_ERROR;
    }
    if (find_case_folded(text, "warning") != NULL) {
        return UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_WARNING;
    }
    if (find_case_folded(text, "note") != NULL ||
        find_case_folded(text, "info") != NULL) {
        return UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_INFO;
    }

    return UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_INFO;
}

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

        if (parsed == 6) {
            (void)snprintf(path, sizeof(path), "%c:%s", drive, tail);
            parsed = 5;
        } else {
            parsed = sscanf(
                line,
                "%c:%2045[^:]:%lu: %63[^:]: %1023[^\n]",
                &drive,
                tail,
                &line_number,
                severity,
                message);
            column_number = 0UL;

            if (parsed == 5) {
                (void)snprintf(path, sizeof(path), "%c:%s", drive, tail);
                parsed = 4;
            }
        }
    } else {
        parsed = sscanf(
            line,
            "%2047[^:]:%lu:%lu: %63[^:]: %1023[^\n]",
            path,
            &line_number,
            &column_number,
            severity,
            message);

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
    if (parsed >= 3) {
        copy_text(problem->location.uri, sizeof(problem->location.uri), path);
        problem->location.line = (size_t)line_number;
        problem->severity =
            umi_developer_diagnostic_severity_from_text(severity);
    }

    *out_matched = 1;
    return UMI_STATUS_OK;
}

static UmiStatus parse_prefix(
    const char *source_id,
    const char *required_token,
    const char *line,
    UmiDeveloperProblem *problem,
    int *out_matched)
{
    if (required_token != NULL &&
        find_case_folded(line, required_token) == NULL) {
        *out_matched = 0;
        return UMI_STATUS_OK;
    }

    init_problem(problem, source_id, line);
    *out_matched = 1;
    return UMI_STATUS_OK;
}

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

    if (parsed != 2) {
        parsed = sscanf(
            line,
            "  File \"%2047[^\"]\", line %lu",
            path,
            &line_number);
    }

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

UmiStatus umi_developer_diagnostic_parse_pattern(
    UmiDeveloperDiagnosticPattern pattern,
    const char *source_id,
    const char *required_token,
    const char *line,
    UmiDeveloperProblem *out_problem,
    int *out_matched)
{
    if (source_id == NULL || line == NULL ||
        out_problem == NULL || out_matched == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_matched = 0;

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
