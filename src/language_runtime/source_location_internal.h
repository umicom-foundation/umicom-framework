/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/source_location_internal.h
 * PURPOSE: Share validated source coordinates and URI syntax between navigation and symbol readers.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_SOURCE_LOCATION_INTERNAL_H
#define UMICOM_LANGUAGE_RUNTIME_SOURCE_LOCATION_INTERNAL_H
#include "umicom/language_runtime/location_catalogue.h"
#include "umicom/language_runtime/json_tree.h"
#include "umicom/language_runtime/json_text.h"
#include <stdlib.h>
#include <string.h>
static UmiStatus LocationMember(const UmiJsonTree *tree, int object, const char *name, int required, int *out)
{
    *out = -1;
    if (UmiJsonTreeKind(tree, object) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        return UMI_STATUS_PARSE_ERROR;
    UmiStatus status = UmiJsonTreeMember(tree, object, name, out);
    if (status == UMI_STATUS_NOT_FOUND)
        return required ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK;
    return status;
}
static int LocationCompare(UmiEditorTextPosition a, UmiEditorTextPosition b)
{
    if (a.line != b.line)
        return a.line < b.line ? -1 : 1;
    return a.utf16_column == b.utf16_column ? 0 : a.utf16_column < b.utf16_column ? -1 : 1;
}
static UmiStatus LocationPosition(const UmiJsonTree *tree, int node, UmiEditorTextPosition *out)
{
    int line, column;
    int64_t row = 0, col = 0;
    UmiStatus status = LocationMember(tree, node, "line", 1, &line);
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, node, "character", 1, &column);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeInteger(tree, line, &row);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeInteger(tree, column, &col);
    if (status == UMI_STATUS_OK && (row < 0 || col < 0 || row > INT32_MAX || col > INT32_MAX))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        *out = (UmiEditorTextPosition){(uint64_t)row, (uint64_t)col};
    return status;
}
static UmiStatus LocationRange(const UmiJsonTree *tree, int node, UmiLanguageSourceRange *out)
{
    int start, end;
    UmiStatus status = LocationMember(tree, node, "start", 1, &start);
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, node, "end", 1, &end);
    if (status == UMI_STATUS_OK)
        status = LocationPosition(tree, start, &out->start);
    if (status == UMI_STATUS_OK)
        status = LocationPosition(tree, end, &out->end);
    if (status == UMI_STATUS_OK && LocationCompare(out->start, out->end) > 0)
        status = UMI_STATUS_PARSE_ERROR;
    return status;
}
static int LocationLetter(unsigned char value)
{
    return (value >= 'A' && value <= 'Z') || (value >= 'a' && value <= 'z');
}
static int LocationHex(unsigned char value)
{
    return (value >= '0' && value <= '9') || (value >= 'a' && value <= 'f') || (value >= 'A' && value <= 'F');
}
/* A raw URI has no JSON escape syntax. URI punctuation is checked first;
 * the shared string scanner then validates UTF-8 without allocating. */
static UmiStatus LocationUriValidate(const char *uri, size_t bytes)
{
    if (bytes > 8192U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const char *colon = memchr(uri, ':', bytes);
    if (bytes == 0U || !LocationLetter((unsigned char)uri[0]) || colon == NULL)
        return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 1U; uri + i < colon; ++i)
    {
        unsigned char ch = (unsigned char)uri[i];
        if (!LocationLetter(ch) && !(ch >= '0' && ch <= '9') && ch != '+' && ch != '-' && ch != '.')
            return UMI_STATUS_PARSE_ERROR;
    }
    for (size_t i = 0U; i < bytes; ++i)
    {
        unsigned char ch = (unsigned char)uri[i];
        if (ch <= 0x20U || ch == 0x7fU || ch == '\\' || ch == '"')
            return UMI_STATUS_PARSE_ERROR;
        if (ch == '%')
        {
            if (bytes - i < 3U || !LocationHex((unsigned char)uri[i + 1U]) ||
                !LocationHex((unsigned char)uri[i + 2U]))
                return UMI_STATUS_PARSE_ERROR;
            i += 2U;
        }
    }
    size_t decoded = 0U;
    return UmiLanguageRuntimeJsonTextSpan(uri, bytes, NULL, 0U, &decoded);
}
/* Symbol and navigation identifiers share one URI syntax check; raw quotes are refused before any host access.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus LocationUri(const UmiJsonTree *tree, int node, char **out)
{
    const char *raw = NULL;
    size_t length = 0U;
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_STRING)
        return UMI_STATUS_PARSE_ERROR;
    UmiStatus status = UmiJsonTreeSourceSpan(tree, node, &raw, &length);
    (void)raw;
    if (status != UMI_STATUS_OK)
        return status;
    char *uri = malloc(length + 1U);
    if (uri == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiJsonTreeText(tree, node, uri, length + 1U);
    size_t bytes = status == UMI_STATUS_OK ? strlen(uri) : 0U;
    if (status == UMI_STATUS_OK && bytes > 8192U)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK)
    {
        /* Keep parsing separate from resource access. A valid identifier does
         * not authorize network access or mean that its target exists. */
        const char *colon = strchr(uri, ':');
        if (bytes == 0U || !LocationLetter((unsigned char)uri[0]) || colon == NULL)
            status = UMI_STATUS_PARSE_ERROR;
        for (size_t i = 1U; status == UMI_STATUS_OK && uri + i < colon; ++i)
        {
            unsigned char ch = (unsigned char)uri[i];
            if (!LocationLetter(ch) && !(ch >= '0' && ch <= '9') && ch != '+' && ch != '-' && ch != '.')
                status = UMI_STATUS_PARSE_ERROR;
        }
        for (size_t i = 0U; status == UMI_STATUS_OK && i < bytes; ++i)
        {
            unsigned char ch = (unsigned char)uri[i];
            if (ch <= 0x20U || ch == 0x7fU || ch == '\\')
                status = UMI_STATUS_PARSE_ERROR;
            else if (ch == '%')
            {
                if (bytes - i < 3U || !LocationHex((unsigned char)uri[i + 1U]) ||
                    !LocationHex((unsigned char)uri[i + 2U]))
                    status = UMI_STATUS_PARSE_ERROR;
                else
                    i += 2U;
            }
        }
    }
    if (status == UMI_STATUS_OK)
        *out = uri;
    else
        free(uri);
    return status;
}
#endif
static UmiStatus LocationUri(const UmiJsonTree *tree, int node, char **out)
{
    const char *raw = NULL;
    size_t length = 0U;
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_STRING)
        return UMI_STATUS_PARSE_ERROR;
    UmiStatus status = UmiJsonTreeSourceSpan(tree, node, &raw, &length);
    (void)raw;
    if (status != UMI_STATUS_OK)
        return status;
    char *uri = malloc(length + 1U);
    if (uri == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiJsonTreeText(tree, node, uri, length + 1U);
    size_t bytes = status == UMI_STATUS_OK ? strlen(uri) : 0U;
    if (status == UMI_STATUS_OK)
        status = LocationUriValidate(uri, bytes);
    if (status == UMI_STATUS_OK)
        *out = uri;
    else
        free(uri);
    return status;
}

#endif
