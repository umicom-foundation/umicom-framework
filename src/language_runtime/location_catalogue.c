/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/location_catalogue.c
 * PURPOSE: Validate complete navigation results and retain source and target ranges in owned storage.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/location_catalogue.h"
#include "umicom/language_runtime/response_tree.h"
#include <stdlib.h>
#include <string.h>
typedef struct LocationEntry
{
    char *uri;
    UmiLanguageSourceLocation value;
} LocationEntry;
struct UmiLanguageLocationCatalogue
{
    LocationEntry *entries;
    size_t count;
};
/* Coordinate and URI decoding now live in source_location_internal.h so
 * symbols and navigation cannot disagree. Keep the original helpers for review. */
#if 0
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
#include "source_location_internal.h"
static UmiStatus LocationRead(const UmiJsonTree *tree, int node, int array, LocationEntry *entry)
{
    int uri, target_uri, range = -1, selection = -1, origin = -1;
    UmiStatus status = LocationMember(tree, node, "uri", 0, &uri);
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, node, "targetUri", 0, &target_uri);
    if (status != UMI_STATUS_OK)
        return status;
    if ((uri < 0) == (target_uri < 0))
        return UMI_STATUS_PARSE_ERROR;
    entry->value.is_link = target_uri >= 0;
    if (entry->value.is_link && !array)
        return UMI_STATUS_PARSE_ERROR;
    if (entry->value.is_link)
    {
        status = LocationMember(tree, node, "targetRange", 1, &range);
        if (status == UMI_STATUS_OK)
            status = LocationMember(tree, node, "targetSelectionRange", 1, &selection);
        if (status == UMI_STATUS_OK)
            status = LocationMember(tree, node, "originSelectionRange", 0, &origin);
    }
    else
        status = LocationMember(tree, node, "range", 1, &range);
    if (status == UMI_STATUS_OK)
        status = LocationUri(tree, entry->value.is_link ? target_uri : uri, &entry->uri);
    if (status == UMI_STATUS_OK)
        status = LocationRange(tree, range, &entry->value.target);
    if (status == UMI_STATUS_OK)
    {
        entry->value.uri = entry->uri;
        if (entry->value.is_link)
            status = LocationRange(tree, selection, &entry->value.selection);
        else
            entry->value.selection = entry->value.target;
    }
    if (status == UMI_STATUS_OK &&
        (LocationCompare(entry->value.selection.start, entry->value.target.start) < 0 ||
         LocationCompare(entry->value.selection.end, entry->value.target.end) > 0))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK && origin >= 0)
    {
        status = LocationRange(tree, origin, &entry->value.origin);
        if (status == UMI_STATUS_OK)
            entry->value.has_origin = 1;
    }
    return status;
}
UmiStatus UmiLanguageLocationCatalogueCreate(const void *json, size_t bytes,
                                             const UmiCancellationToken *cancel,
                                             UmiLanguageLocationCatalogue **out_catalogue)
{
    if (out_catalogue == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_catalogue = NULL;
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {1024U * 1024U, 131072U, 16U};
    UmiStatus status = UmiJsonTreeCreate(json, bytes, &limits, cancel, &tree);
    if (status != UMI_STATUS_OK)
        return status;
    UmiLanguageLocationCatalogue *catalogue = calloc(1U, sizeof(*catalogue));
    if (catalogue == NULL)
    {
        UmiJsonTreeDestroy(tree);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    int array = UmiJsonTreeKind(tree, 0) == UMI_LANGUAGE_RUNTIME_JSON_ARRAY;
    catalogue->count = UmiJsonTreeIsNull(tree, 0) ? 0U : array ? UmiJsonTreeCount(tree, 0) : 1U;
    if (catalogue->count > 4096U)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK && catalogue->count != 0U)
    {
        catalogue->entries = calloc(catalogue->count, sizeof(*catalogue->entries));
        if (catalogue->entries == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
    }
    int node = array ? UmiJsonTreeFirst(tree, 0) : 0;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < catalogue->count;
         ++i, node = UmiJsonTreeNext(tree, node))
    {
        if (umi_cancellation_token_is_requested(cancel))
            status = UMI_STATUS_CANCELLED;
        else
            status = LocationRead(tree, node, array, &catalogue->entries[i]);
        if (status == UMI_STATUS_OK && i != 0U &&
            catalogue->entries[i].value.is_link != catalogue->entries[0].value.is_link)
            status = UMI_STATUS_PARSE_ERROR;
    }
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    UmiJsonTreeDestroy(tree);
    if (status == UMI_STATUS_OK)
        *out_catalogue = catalogue;
    else
        UmiLanguageLocationCatalogueDestroy(catalogue);
    return status;
}
UmiStatus UmiLanguageLocationCatalogueReadResponse(const void *json, size_t bytes, uint64_t expected_id,
                                                   const UmiCancellationToken *cancel,
                                                   UmiLanguageLocationCatalogue **out_catalogue)
{
    if (out_catalogue == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_catalogue = NULL;
    UmiJsonTreeLimits limits = {1024U * 1024U, 131072U, 16U};
    UmiJsonTree *tree = NULL;
    int result = -1;
    UmiStatus status = UmiLanguageResponseTreeRead(json, bytes, expected_id, &limits, cancel, &tree, &result);
    const char *value = NULL;
    size_t length = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeSourceSpan(tree, result, &value, &length);
    if (status == UMI_STATUS_OK)
        status = UmiLanguageLocationCatalogueCreate(value, length, cancel, out_catalogue);
    UmiJsonTreeDestroy(tree);
    return status;
}
void UmiLanguageLocationCatalogueDestroy(UmiLanguageLocationCatalogue *catalogue)
{
    if (catalogue != NULL)
    {
        if (catalogue->entries != NULL)
            for (size_t i = 0U; i < catalogue->count; ++i)
                free(catalogue->entries[i].uri);
        free(catalogue->entries);
        free(catalogue);
    }
}
size_t UmiLanguageLocationCatalogueCount(const UmiLanguageLocationCatalogue *catalogue)
{
    return catalogue == NULL ? 0U : catalogue->count;
}
UmiStatus UmiLanguageLocationCatalogueAt(const UmiLanguageLocationCatalogue *catalogue, size_t index,
                                         UmiLanguageSourceLocation *out_location)
{
    if (catalogue == NULL || out_location == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= catalogue->count)
        return UMI_STATUS_NOT_FOUND;
    *out_location = catalogue->entries[index].value;
    return UMI_STATUS_OK;
}

#include "selection_locations.inc"

#include "folding_locations.inc"
