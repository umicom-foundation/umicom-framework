/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/diagnostic_parsers/ctest.c
 *
 * PURPOSE:
 *   Normalize CTest output into the shared Framework Problems model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/diagnostic_parsers/ctest.h"

#include "umicom/developer_productivity/diagnostic_parser_support.h"
#include <string.h>

/* Provide the parse line operation used by this module and its client applications. */
/* The former token search could count summary text as a failure and miss
 * exception or timeout rows. Retain it for review; the result-row parser below
 * preserves the original parser interface and recognises actual CTest outcomes. */
#if 0
static UmiStatus parse_line(
    const char *line,
    UmiDeveloperProblem *out_problem,
    int *out_matched)
{
    return umi_developer_diagnostic_parse_pattern(
        UMI_DEVELOPER_DIAGNOSTIC_PREFIX,
        "developer.diagnostic.ctest",
        "Failed",
        line,
        out_problem,
        out_matched);
}

#endif

/* SGR colour is presentation, not evidence of a test outcome. Bound both the
 * visible line and escape overhead; incomplete or unrelated control sequences
 * are rejected rather than allowing a partial result into the Problems model. */
static UmiStatus CtestVisibleLine(const char *line, char *text, size_t capacity)
{
    size_t read = 0U, written = 0U;
    while (line[read] != '\0') {
        if (read >= capacity * 4U) return UMI_STATUS_CAPACITY_EXCEEDED;
        if ((unsigned char)line[read] == 0x1bU) {
            ++read;
            if (line[read] != '[') return UMI_STATUS_PARSE_ERROR;
            ++read;
            while ((line[read] >= '0' && line[read] <= '9') || line[read] == ';') {
                if (read >= capacity * 4U) return UMI_STATUS_CAPACITY_EXCEEDED;
                ++read;
            }
            if (line[read] != 'm') return UMI_STATUS_PARSE_ERROR;
            ++read;
            continue;
        }
        if (written + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        text[written++] = line[read++];
    }
    while (written != 0U && (text[written - 1U] == '\r' || text[written - 1U] == '\n'))
        --written;
    text[written] = '\0';
    return UMI_STATUS_OK;
}
static const char *CtestSpaces(const char *cursor)
{
    while (*cursor == ' ' || *cursor == '\t') ++cursor;
    return cursor;
}
static int CtestNumber(const char **cursor, uint64_t *number)
{
    const char *at = *cursor;
    uint64_t value = 0U;
    if (*at < '0' || *at > '9') return 0;
    do {
        uint64_t digit = (uint64_t)(*at - '0');
        if (value > (UINT64_MAX - digit) / 10U) return 0;
        value = value * 10U + digit;
        ++at;
    } while (*at >= '0' && *at <= '9');
    *number = value; *cursor = at;
    return value != 0U;
}
static int CtestWord(const char *text, const char *word)
{
    size_t length = strlen(word);
    return strncmp(text, word, length) == 0 &&
        (text[length] == '\0' || text[length] == ' ' || text[length] == '\t');
}
/* Preserve the result category instead of deriving severity from arbitrary
 * words in the test name. Skipped/disabled tests describe missing coverage;
 * exceptions, timeouts and not-run tests remain adverse outcomes. */
static const char *CtestOutcome(const char *text, UmiDeveloperProductivitySeverity *severity)
{
    *severity = UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_ERROR;
    if (strncmp(text, "Exit code ", 10U) == 0) {
        const char *code = text + 10U;
        if (strncmp(code, "0x", 2U) != 0) return NULL;
        code += 2U;
        const char *first = code;
        while ((*code >= '0' && *code <= '9') || (*code >= 'a' && *code <= 'f') ||
               (*code >= 'A' && *code <= 'F')) ++code;
        if (code == first) return NULL;
        text = CtestSpaces(code);
        if (strncmp(text, "***Exception:", 13U) != 0) return NULL;
        return "ctest.exception";
    }
    if (strncmp(text, "***", 3U) != 0) return NULL;
    text += 3U;
    if (CtestWord(text, "Failed")) return "ctest.failed";
    if (CtestWord(text, "Timeout")) return "ctest.timeout";
    if (CtestWord(text, "Not Run (Disabled)")) {
        *severity = UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_WARNING;
        return "ctest.disabled";
    }
    if (CtestWord(text, "Not Run")) return "ctest.not-run";
    if (strncmp(text, "Exception:", 10U) == 0) return "ctest.exception";
    if (CtestWord(text, "Skipped") || CtestWord(text, "Disabled")) {
        *severity = UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_WARNING;
        return CtestWord(text, "Skipped") ? "ctest.skipped" : "ctest.disabled";
    }
    return NULL;
}
static UmiStatus parse_line(const char *line, UmiDeveloperProblem *out_problem, int *out_matched)
{
    if (line == NULL || out_problem == NULL || out_matched == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_matched = 0;
    char visible[UMI_DEVELOPER_PRODUCTIVITY_TEXT_CAPACITY];
    UmiStatus status = CtestVisibleLine(line, visible, sizeof visible);
    if (status != UMI_STATUS_OK) return status;
    const char *at = CtestSpaces(visible);
    if (*at >= '0' && *at <= '9') {
        uint64_t done, total;
        if (!CtestNumber(&at, &done) || *at++ != '/' ||
            !CtestNumber(&at, &total) || done > total) return UMI_STATUS_OK;
        at = CtestSpaces(at);
    }
    /* A test number alone is not a source-code line. Keep location empty so
     * Studio never navigates to a fabricated file based on CTest's ordinal. */
    if (strncmp(at, "Test", 4U) != 0 || (at[4] != ' ' && at[4] != '\t')) return UMI_STATUS_OK;
    at = CtestSpaces(at + 4U);
    if (*at++ != '#') return UMI_STATUS_OK;
    uint64_t number;
    if (!CtestNumber(&at, &number) || *at++ != ':') return UMI_STATUS_OK;
    const char *name = CtestSpaces(at);
    const char *separator = name;
    for (;;) {
        separator = strpbrk(separator, " \t");
        if (separator == NULL) return UMI_STATUS_OK;
        const char *dots = CtestSpaces(separator), *end = dots;
        while (*end == '.') ++end;
        if (end - dots >= 2 && separator != name) {
            UmiDeveloperProductivitySeverity severity;
            const char *code = CtestOutcome(CtestSpaces(end), &severity);
            if (code != NULL) {
                UmiDeveloperProblem candidate = {0};
                strcpy(candidate.source, "developer.diagnostic.ctest");
                strcpy(candidate.code, code);
                strcpy(candidate.message, visible);
                candidate.severity = severity;
                candidate.transient = 1;
                candidate.revision = 1U;
                *out_problem = candidate;
                *out_matched = 1;
                return UMI_STATUS_OK;
            }
        }
        ++separator;
    }
}

/*
 * Provide the developer diagnostic parser ctest operation used by this module and its
 * client applications.
 */
const UmiDeveloperDiagnosticParser *umi_developer_diagnostic_parser_ctest(void)
{
    static const UmiDeveloperDiagnosticParser parser = {
        .structure_size = (uint32_t)sizeof(UmiDeveloperDiagnosticParser),
        .api_version = UMI_DEVELOPER_PRODUCTIVITY_API_VERSION,
        .parser_id = "developer.parser.ctest",
        .title = "CTest",
        .tool_id = "ctest",
        .priority = 100,
        .parse = parse_line
    };
    return &parser;
}
