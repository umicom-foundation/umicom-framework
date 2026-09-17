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
