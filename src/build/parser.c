/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/parser.c
 *
 * PURPOSE:
 *   Implement portable parsing of common compiler and build-tool diagnostic formats.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/parser.h"
#include "umicom/diagnostics/compiler_parser.h"

#include <string.h>

/* The previous line-only projection and scan remain for review. The active
 * implementation below shares one checked projection and groups CMake records,
 * so build history and Studio's Problems list retain the same explanation. */
#if 0
/* Preserve this public build API while reusing Framework's compiler grammar. */
UmiStatus umi_build_parse_diagnostic_line(const char *line,
    UmiBuildDiagnostic *out_diagnostic)
{
    UmiCompilerDiagnosticFields fields;
    UmiBuildDiagnostic candidate = {0};
    UmiStatus status;
    if (line == NULL || out_diagnostic == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_diagnostic, 0, sizeof *out_diagnostic);
    status = UmiCompilerDiagnosticParseText(line, &fields);
    if (status != UMI_STATUS_OK) return status;
    if (strlen(fields.path) >= sizeof candidate.file || strlen(fields.code) >= sizeof candidate.code ||
        strlen(fields.message) >= sizeof candidate.message) return UMI_STATUS_CAPACITY_EXCEEDED;
    strcpy(candidate.file, fields.path); strcpy(candidate.message, fields.message); strcpy(candidate.code, fields.code);
    candidate.line = fields.line; candidate.column = fields.column;
    switch (fields.severity) {
        case UMI_DIAGNOSTIC_FATAL: candidate.severity = UMI_BUILD_DIAGNOSTIC_FATAL; break;
        case UMI_DIAGNOSTIC_ERROR: candidate.severity = UMI_BUILD_DIAGNOSTIC_ERROR; break;
        case UMI_DIAGNOSTIC_WARNING: candidate.severity = UMI_BUILD_DIAGNOSTIC_WARNING; break;
        default: candidate.severity = UMI_BUILD_DIAGNOSTIC_NOTE; break;
    }
    *out_diagnostic = candidate;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_parse_output(const char *output, UmiBuildDiagnosticList *out_list)
{
    const char *cursor;
    if (output == NULL || out_list == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_build_diagnostic_list_init(out_list);
    cursor = output;
    while (*cursor != '\0') {
        const char *end = strchr(cursor, '\n');
        size_t length = end != NULL ? (size_t)(end - cursor) : strlen(cursor);
        char line[8192];
        UmiBuildDiagnostic diagnostic;
        UmiStatus status;
        if (length >= sizeof line) {
            /* Count an unparsed line instead of presenting cropped evidence. */
            ++out_list->dropped;
        } else {
            memcpy(line, cursor, length); line[length] = '\0';
            status = umi_build_parse_diagnostic_line(line, &diagnostic);
            if (status == UMI_STATUS_OK) (void)umi_build_diagnostic_list_add(out_list, &diagnostic);
            else if (status == UMI_STATUS_CAPACITY_EXCEEDED) ++out_list->dropped;
        }
        if (end == NULL) break;
        cursor = end + 1;
    }
    return UMI_STATUS_OK;
}
#endif

/* Both line and transcript entry points project the shared grammar into the
 * existing build record. Capacity checks prevent navigation to a cropped path. */
static UmiStatus BuildDiagnosticFromFields(
    const UmiCompilerDiagnosticFields *fields, UmiBuildDiagnostic *outDiagnostic)
{
    UmiBuildDiagnostic candidate = {0};
    if (strlen(fields->path) >= sizeof candidate.file ||
        strlen(fields->code) >= sizeof candidate.code ||
        strlen(fields->message) >= sizeof candidate.message)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    strcpy(candidate.file, fields->path);
    strcpy(candidate.code, fields->code);
    strcpy(candidate.message, fields->message);
    candidate.line = fields->line;
    candidate.column = fields->column;
    switch (fields->severity) {
        case UMI_DIAGNOSTIC_FATAL: candidate.severity = UMI_BUILD_DIAGNOSTIC_FATAL; break;
        case UMI_DIAGNOSTIC_ERROR: candidate.severity = UMI_BUILD_DIAGNOSTIC_ERROR; break;
        case UMI_DIAGNOSTIC_WARNING: candidate.severity = UMI_BUILD_DIAGNOSTIC_WARNING; break;
        default: candidate.severity = UMI_BUILD_DIAGNOSTIC_NOTE; break;
    }
    *outDiagnostic = candidate;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_parse_diagnostic_line(const char *line,
    UmiBuildDiagnostic *out_diagnostic)
{
    if (line == NULL || out_diagnostic == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the established single-line API's cleared output on a miss. */
    memset(out_diagnostic, 0, sizeof *out_diagnostic);
    UmiCompilerDiagnosticFields fields;
    UmiStatus status = UmiCompilerDiagnosticParseText(line, &fields);
    return status == UMI_STATUS_OK
        ? BuildDiagnosticFromFields(&fields, out_diagnostic) : status;
}

UmiStatus umi_build_parse_output(const char *output, UmiBuildDiagnosticList *out_list)
{
    if (output == NULL || out_list == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_build_diagnostic_list_init(out_list);
    const char *cursor = output;
    while (*cursor != '\0') {
        UmiCompilerDiagnosticFields fields;
        UmiBuildDiagnostic diagnostic;
        size_t consumed = 0U;
        /* Keep the former physical-line bound, including raw colour bytes.
         * A long ordinary line must still contribute to the dropped count. */
        const char *newline = strchr(cursor, '\n');
        size_t length = newline != NULL ? (size_t)(newline - cursor) : strlen(cursor);
        UmiStatus status;
        if (length >= 8192U) {
            consumed = length + (newline != NULL ? 1U : 0U);
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        } else {
            status = UmiCompilerDiagnosticParseBlock(cursor, &fields, &consumed);
        }
        if (status == UMI_STATUS_OK)
            status = BuildDiagnosticFromFields(&fields, &diagnostic);
        if (status == UMI_STATUS_OK)
            (void)umi_build_diagnostic_list_add(out_list, &diagnostic);
        else if (status == UMI_STATUS_CAPACITY_EXCEEDED)
            ++out_list->dropped;
        /* Nonempty valid input always consumes a record. This guard keeps an
         * unexpected future parser regression from hanging a build reviewer. */
        if (consumed == 0U) return UMI_STATUS_INVALID_STATE;
        cursor += consumed;
    }
    return UMI_STATUS_OK;
}

/* Frontends and observed runners share the same checked projection as complete
 * transcripts; no separate compiler grammar belongs in an application. */
/* The shared projection now validates fixed-array terminators and severity before reading caller-owned fields. Retain the previous public boundary for review. The previous implementation is retained for engineering review. */
#if 0
UmiStatus UmiBuildDiagnosticFromCompilerFields(const UmiCompilerDiagnosticFields *fields,
    UmiBuildDiagnostic *out)
{
    if (fields == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return BuildDiagnosticFromFields(fields, out);
}
#endif
UmiStatus UmiBuildDiagnosticFromCompilerFields(const UmiCompilerDiagnosticFields *fields,
    UmiBuildDiagnostic *out)
{
    if (fields == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Public callers can supply fixed arrays that are not C strings. Verify
     * their bounds before the shared projection uses strlen or copies a path. */
    if (memchr(fields->path, '\0', sizeof fields->path) == NULL ||
        memchr(fields->code, '\0', sizeof fields->code) == NULL ||
        memchr(fields->message, '\0', sizeof fields->message) == NULL ||
        fields->severity < UMI_DIAGNOSTIC_TRACE || fields->severity > UMI_DIAGNOSTIC_FATAL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return BuildDiagnosticFromFields(fields, out);
}
