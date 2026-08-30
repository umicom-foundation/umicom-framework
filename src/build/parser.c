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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static UmiBuildDiagnosticSeverity severity_from_text(const char *text)
{
    if (text == NULL) {
        return UMI_BUILD_DIAGNOSTIC_NOTE;
    }
    if (strstr(text, "fatal error") != NULL) {
        return UMI_BUILD_DIAGNOSTIC_FATAL;
    }
    if (strstr(text, "error") != NULL) {
        return UMI_BUILD_DIAGNOSTIC_ERROR;
    }
    if (strstr(text, "warning") != NULL) {
        return UMI_BUILD_DIAGNOSTIC_WARNING;
    }
    return UMI_BUILD_DIAGNOSTIC_NOTE;
}

static void trim_newline(char *text)
{
    size_t length;
    if (text == NULL) {
        return;
    }
    length = strlen(text);
    while (length > 0U &&
           (text[length - 1U] == '\n' || text[length - 1U] == '\r')) {
        text[--length] = '\0';
    }
}

UmiStatus umi_build_parse_diagnostic_line(const char *line,
                                          UmiBuildDiagnostic *out_diagnostic)
{
    char file[UMI_BUILD_PATH_CAPACITY];
    char severity[64];
    char message[UMI_BUILD_DIAGNOSTIC_MESSAGE_CAPACITY];
    unsigned long line_number = 0UL;
    unsigned long column_number = 0UL;
    int matched;

    if (line == NULL || out_diagnostic == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_diagnostic, 0, sizeof(*out_diagnostic));
    (void)memset(file, 0, sizeof(file));
    (void)memset(severity, 0, sizeof(severity));
    (void)memset(message, 0, sizeof(message));

    matched = sscanf(line,
                     "%2047[^:]:%lu:%lu: %63[^:]: %1023[^\n]",
                     file,
                     &line_number,
                     &column_number,
                     severity,
                     message);
    if (matched == 5) {
        (void)snprintf(out_diagnostic->file,
                       sizeof(out_diagnostic->file),
                       "%s",
                       file);
        out_diagnostic->line = (size_t)line_number;
        out_diagnostic->column = (size_t)column_number;
        out_diagnostic->severity = severity_from_text(severity);
        (void)snprintf(out_diagnostic->message,
                       sizeof(out_diagnostic->message),
                       "%s",
                       message);
        trim_newline(out_diagnostic->message);
        return UMI_STATUS_OK;
    }

    matched = sscanf(line,
                     "%2047[^(:](%lu,%lu): %63[^:]: %1023[^\n]",
                     file,
                     &line_number,
                     &column_number,
                     severity,
                     message);
    if (matched == 5) {
        (void)snprintf(out_diagnostic->file,
                       sizeof(out_diagnostic->file),
                       "%s",
                       file);
        out_diagnostic->line = (size_t)line_number;
        out_diagnostic->column = (size_t)column_number;
        out_diagnostic->severity = severity_from_text(severity);
        (void)snprintf(out_diagnostic->message,
                       sizeof(out_diagnostic->message),
                       "%s",
                       message);
        trim_newline(out_diagnostic->message);
        return UMI_STATUS_OK;
    }

    if (strstr(line, "error") != NULL ||
        strstr(line, "warning") != NULL) {
        out_diagnostic->severity = severity_from_text(line);
        (void)snprintf(out_diagnostic->message,
                       sizeof(out_diagnostic->message),
                       "%s",
                       line);
        trim_newline(out_diagnostic->message);
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_build_parse_output(const char *output,
                                 UmiBuildDiagnosticList *out_list)
{
    const char *cursor;
    const char *end;
    char line[4096];
    size_t length;
    UmiBuildDiagnostic diagnostic;

    if (output == NULL || out_list == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_build_diagnostic_list_init(out_list);
    cursor = output;
    while (*cursor != '\0') {
        end = strchr(cursor, '\n');
        length = end != NULL ? (size_t)(end - cursor) : strlen(cursor);
        if (length >= sizeof(line)) {
            length = sizeof(line) - 1U;
        }
        (void)memcpy(line, cursor, length);
        line[length] = '\0';
        if (umi_build_parse_diagnostic_line(line, &diagnostic) ==
            UMI_STATUS_OK) {
            (void)umi_build_diagnostic_list_add(out_list, &diagnostic);
        }
        if (end == NULL) {
            break;
        }
        cursor = end + 1;
    }
    return UMI_STATUS_OK;
}
