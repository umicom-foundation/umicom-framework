/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/compiler_parser.c
 *
 * PURPOSE:
 *   Implement the compiler parser behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/compiler_parser.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Provide the compiler hash operation used by this module and its client applications. */
static uint64_t compiler_hash(const char *text)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    const unsigned char *cursor = (const unsigned char *)text;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (cursor != NULL && *cursor != 0U) {
        hash ^= (uint64_t)*cursor++;
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

/* Provide the copy range operation used by this module and its client applications. */
static int copy_range(char *destination, size_t capacity,
                      const char *begin, const char *end)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || begin == NULL || end == NULL || end < begin) return 0;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (begin < end && (*begin == ' ' || *begin == '\t')) ++begin;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (end > begin && (end[-1] == ' ' || end[-1] == '\t')) --end;
    length = (size_t)(end - begin);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return 0;
    (void)memcpy(destination, begin, length);
    destination[length] = '\0';
    return 1;
}

/*
 * Provide the parse number range operation used by this module and its client
 * applications.
 */
static int parse_number_range(const char *begin, const char *end, uint32_t *out_value)
{
    char buffer[32];
    char *parse_end;
    unsigned long value;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!copy_range(buffer, sizeof(buffer), begin, end) || buffer[0] == '\0') return 0;
    value = strtoul(buffer, &parse_end, 10);
    /* Apply this branch only when its contract condition is satisfied. */
    if (*parse_end != '\0' || value > UINT32_MAX) return 0;
    *out_value = (uint32_t)value;
    return 1;
}

/* Provide the find last before operation used by this module and its client applications. */
static const char *find_last_before(const char *begin, const char *end, char target)
{
    const char *cursor = end;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (cursor > begin) {
        --cursor;
        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == target) return cursor;
    }
    return NULL;
}

/* Provide the find severity operation used by this module and its client applications. */
static const char *find_severity(const char *text, UmiDiagnosticSeverity *severity,
                                 const char **out_code)
{
    static const struct SeverityMarker {
        const char *marker;
        UmiDiagnosticSeverity severity;
        const char *code;
    } markers[] = {
        { "fatal error:", UMI_DIAGNOSTIC_FATAL, "fatal-error" },
        { "error:", UMI_DIAGNOSTIC_ERROR, "error" },
        { "warning:", UMI_DIAGNOSTIC_WARNING, "warning" },
        { "note:", UMI_DIAGNOSTIC_INFO, "note" }
    };
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < sizeof(markers) / sizeof(markers[0]); ++index) {
        const char *found = strstr(text, markers[index].marker);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (found != NULL) {
            *severity = markers[index].severity;
            *out_code = markers[index].code;
            return found;
        }
    }
    return NULL;
}

/*
 * Provide the parse gnu location operation used by this module and its client
 * applications.
 */
static int parse_gnu_location(const char *text, const char *marker,
                              UmiDiagnosticSnapshot *diagnostic)
{
    const char *location_end = marker;
    const char *column_separator;
    const char *line_separator;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (location_end > text && (location_end[-1] == ' ' || location_end[-1] == ':')) --location_end;
    column_separator = find_last_before(text, location_end, ':');
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (column_separator == NULL ||
        !parse_number_range(column_separator + 1, location_end, &diagnostic->column)) return 0;
    line_separator = find_last_before(text, column_separator, ':');
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (line_separator == NULL ||
        !parse_number_range(line_separator + 1, column_separator, &diagnostic->line)) return 0;
    return copy_range(diagnostic->uri, sizeof(diagnostic->uri), text, line_separator);
}

/*
 * Provide the parse msvc location operation used by this module and its client
 * applications.
 */
static int parse_msvc_location(const char *text, const char *marker,
                               UmiDiagnosticSnapshot *diagnostic)
{
    const char *close = find_last_before(text, marker, ')');
    const char *open;
    const char *comma;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (close == NULL) return 0;
    open = find_last_before(text, close, '(');
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (open == NULL) return 0;
    comma = memchr(open + 1, ',', (size_t)(close - open - 1));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (comma == NULL) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (!parse_number_range(open + 1, close, &diagnostic->line)) return 0;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (!parse_number_range(open + 1, comma, &diagnostic->line) ||
               !parse_number_range(comma + 1, close, &diagnostic->column)) {
        return 0;
    }
    return copy_range(diagnostic->uri, sizeof(diagnostic->uri), text, open);
}

/*
 * Read compiler diagnostic into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_compiler_diagnostic_parse(const UmiOutputRecord *output,
                                        UmiDiagnosticSnapshot *out_diagnostic,
                                        int *out_matched,
                                        void *user_data)
{
    UmiDiagnosticSeverity severity;
    const char *code;
    const char *marker;
    const char *message;
    char identifier[UMI_DIAGNOSTIC_ID_CAPACITY];
    int has_location;
    (void)user_data;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (output == NULL || out_diagnostic == NULL || out_matched == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_matched = 0;
    marker = find_severity(output->text, &severity, &code);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (marker == NULL) return UMI_STATUS_OK;
    message = strchr(marker, ':');
    message = message != NULL ? message + 1 : marker;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*message == ' ' || *message == '\t') ++message;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (*message == '\0') return UMI_STATUS_OK;
    (void)snprintf(identifier, sizeof(identifier), "compiler-%016" PRIx64,
                   compiler_hash(output->text) ^ output->correlation_id);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (umi_diagnostic_snapshot_init(out_diagnostic, identifier, severity,
                                     UMI_DIAGNOSTIC_KIND_COMPILER,
                                     output->source[0] != '\0' ? output->source : "compiler",
                                     message) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)snprintf(out_diagnostic->code, sizeof(out_diagnostic->code), "%s", code);
    out_diagnostic->correlation_id = output->correlation_id;
    out_diagnostic->timestamp_ns = output->timestamp_ns;
    has_location = parse_gnu_location(output->text, marker, out_diagnostic);
    /* Apply this branch only when its contract condition is satisfied. */
    if (has_location == 0) has_location = parse_msvc_location(output->text, marker, out_diagnostic);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (has_location == 0 && strstr(output->text, "link") != NULL) {
        out_diagnostic->kind = UMI_DIAGNOSTIC_KIND_LINKER;
    }
    *out_matched = 1;
    return UMI_STATUS_OK;
}

/*
 * Provide the compiler diagnostic parser operation used by this module and its client
 * applications.
 */
UmiDiagnosticParser umi_compiler_diagnostic_parser(void)
{
    UmiDiagnosticParser parser = { "umicom.compiler", 100, umi_compiler_diagnostic_parse, NULL };
    return parser;
}
