/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/symbol_catalogue.c
 * PURPOSE: Retain complete source-symbol trees with explicit ownership and bounded hierarchy traversal.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/symbol_catalogue.h"
#include "umicom/language_runtime/response_tree.h"
#include "source_location_internal.h"
typedef struct SymbolEntry
{
    UmiLanguageSymbol value;
    char *name, *detail, *container, *uri;
} SymbolEntry;
struct UmiLanguageSymbolCatalogue
{
    SymbolEntry *entries;
    size_t count, capacity;
    char *source_uri;
    int shape;
};
/* Source outlines and call relationships now share decoded-label validation.
 * Keep these original helpers for review of the shared implementation. */
#if 0
static UmiStatus SymbolText(const UmiJsonTree *tree, int object, const char *name, int required, char **out)
{
    int node = -1;
    UmiStatus status = LocationMember(tree, object, name, required, &node);
    if (status != UMI_STATUS_OK || node < 0)
        return status;
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_STRING)
        return UMI_STATUS_PARSE_ERROR;
    const char *raw = NULL;
    size_t bytes = 0U;
    status = UmiJsonTreeSourceSpan(tree, node, &raw, &bytes);
    (void)raw;
    char *text = status == UMI_STATUS_OK ? malloc(bytes + 1U) : NULL;
    if (status == UMI_STATUS_OK && text == NULL)
        status = UMI_STATUS_OUT_OF_MEMORY;
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeText(tree, node, text, bytes + 1U);
    if (status == UMI_STATUS_OK && strlen(text) > 4096U)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK)
        *out = text;
    else
        free(text);
    return status;
}
/* Names containing only Unicode whitespace cannot be useful list entries.
 * The tree already validated UTF-8, so this bounded decoder only classifies
 * scalar values. Keep the whitespace set independent of the process locale. */
static int SymbolVisibleName(const char *text)
{
    const unsigned char *p = (const unsigned char *)text;
    while (*p != '\0')
    {
        uint32_t value = *p++;
        unsigned extra = 0U;
        if (value >= 0xf0U)
        {
            value &= 7U;
            extra = 3U;
        }
        else if (value >= 0xe0U)
        {
            value &= 15U;
            extra = 2U;
        }
        else if (value >= 0xc0U)
        {
            value &= 31U;
            extra = 1U;
        }
        while (extra != 0U)
        {
            value = (value << 6U) | (*p++ & 63U);
            --extra;
        }
        int whitespace = (value >= 9U && value <= 13U) || value == 32U || value == 0x85U || value == 0xa0U ||
                         value == 0x1680U || (value >= 0x2000U && value <= 0x200aU) || value == 0x2028U ||
                         value == 0x2029U || value == 0x202fU || value == 0x205fU || value == 0x3000U;
        if (!whitespace)
            return 1;
    }
    return 0;
}
#endif
#include "symbol_text_internal.h"
static UmiStatus SymbolAppend(UmiLanguageSymbolCatalogue *catalogue, size_t *out)
{
    if (catalogue->count == 4096U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (catalogue->count == catalogue->capacity)
    {
        size_t capacity = catalogue->capacity == 0U ? 32U : catalogue->capacity * 2U;
        SymbolEntry *entries = realloc(catalogue->entries, capacity * sizeof(*entries));
        if (entries == NULL)
            return UMI_STATUS_OUT_OF_MEMORY;
        catalogue->entries = entries;
        catalogue->capacity = capacity;
    }
    *out = catalogue->count++;
    memset(&catalogue->entries[*out], 0, sizeof(catalogue->entries[*out]));
    return UMI_STATUS_OK;
}
static UmiStatus SymbolRead(UmiLanguageSymbolCatalogue *catalogue, const UmiJsonTree *tree, int node,
                            size_t parent, size_t depth, const UmiCancellationToken *cancel)
{
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    if (depth >= 32U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    int location = -1, range = -1, selection = -1, children = -1, kind = -1, tags = -1, deprecated = -1;
    UmiStatus status = LocationMember(tree, node, "location", 0, &location);
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, node, "range", 0, &range);
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, node, "selectionRange", 0, &selection);
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, node, "children", 0, &children);
    if (status != UMI_STATUS_OK)
        return status;
    int hierarchical = location < 0;
    if ((hierarchical && (range < 0 || selection < 0)) ||
        (!hierarchical && (range >= 0 || selection >= 0 || children >= 0)) ||
        (parent != SIZE_MAX && !hierarchical))
        return UMI_STATUS_PARSE_ERROR;
    int shape = hierarchical ? 1 : 2;
    if (catalogue->shape != 0 && catalogue->shape != shape)
        return UMI_STATUS_PARSE_ERROR;
    catalogue->shape = shape;
    size_t index = 0U;
    status = SymbolAppend(catalogue, &index);
    if (status != UMI_STATUS_OK)
        return status;
    SymbolEntry *entry = &catalogue->entries[index];
    entry->value.parent = parent;
    entry->value.depth = depth;
    entry->value.hierarchical = hierarchical;
    status = SymbolText(tree, node, "name", 1, &entry->name);
    if (status == UMI_STATUS_OK && !SymbolVisibleName(entry->name))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = SymbolText(tree, node, "detail", 0, &entry->detail);
    if (status == UMI_STATUS_OK)
        status = SymbolText(tree, node, "containerName", 0, &entry->container);
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, node, "kind", 1, &kind);
    int64_t integer = 0;
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeInteger(tree, kind, &integer);
    if (status == UMI_STATUS_OK && (integer < 1 || integer > INT32_MAX))
        status = UMI_STATUS_PARSE_ERROR;
    entry->value.kind = (int32_t)(status == UMI_STATUS_OK ? integer : 0);
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, node, "deprecated", 0, &deprecated);
    if (status == UMI_STATUS_OK && deprecated >= 0)
        status = UmiJsonTreeBoolean(tree, deprecated, &entry->value.deprecated);
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, node, "tags", 0, &tags);
    if (status == UMI_STATUS_OK && tags >= 0)
    {
        if (UmiJsonTreeKind(tree, tags) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
            status = UMI_STATUS_PARSE_ERROR;
        for (int tag = UmiJsonTreeFirst(tree, tags); status == UMI_STATUS_OK && tag >= 0;
             tag = UmiJsonTreeNext(tree, tag))
        {
            status = UmiJsonTreeInteger(tree, tag, &integer);
            if (status == UMI_STATUS_OK && (integer < 1 || integer > INT32_MAX))
                status = UMI_STATUS_PARSE_ERROR;
            if (status == UMI_STATUS_OK && integer == 1)
                entry->value.deprecated = 1;
        }
    }
    if (status == UMI_STATUS_OK && hierarchical)
    {
        status = LocationRange(tree, range, &entry->value.location.target);
        if (status == UMI_STATUS_OK)
            status = LocationRange(tree, selection, &entry->value.location.selection);
        if (status == UMI_STATUS_OK &&
            (LocationCompare(entry->value.location.selection.start, entry->value.location.target.start) < 0 ||
             LocationCompare(entry->value.location.selection.end, entry->value.location.target.end) > 0))
            status = UMI_STATUS_PARSE_ERROR;
        entry->value.location.uri = catalogue->source_uri;
    }
    else if (status == UMI_STATUS_OK)
    {
        int uri = -1;
        status = LocationMember(tree, location, "uri", 1, &uri);
        if (status == UMI_STATUS_OK)
            status = LocationMember(tree, location, "range", 1, &range);
        if (status == UMI_STATUS_OK)
            status = LocationUri(tree, uri, &entry->uri);
        if (status == UMI_STATUS_OK)
            status = LocationRange(tree, range, &entry->value.location.target);
        /* Flat information supplies a reveal range rather than an identifier
         * selection. Open its starting caret; retain the full range separately. */
        entry->value.location.selection.start = entry->value.location.target.start;
        entry->value.location.selection.end = entry->value.location.target.start;
        entry->value.location.uri = entry->uri;
    }
    entry->value.name = entry->name;
    entry->value.detail = entry->detail == NULL ? "" : entry->detail;
    entry->value.container = entry->container == NULL ? "" : entry->container;
    if (status == UMI_STATUS_OK && children >= 0)
    {
        if (UmiJsonTreeKind(tree, children) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
            return UMI_STATUS_PARSE_ERROR;
        /* Recursive appends may relocate entries. Use the stable parent index,
         * never an entry pointer, after descending into a child. */
        for (int child = UmiJsonTreeFirst(tree, children); status == UMI_STATUS_OK && child >= 0;
             child = UmiJsonTreeNext(tree, child))
            status = SymbolRead(catalogue, tree, child, index, depth + 1U, cancel);
    }
    return status;
}
UmiStatus UmiLanguageSymbolCatalogueCreate(const void *json, size_t bytes, const char *document_uri,
                                           const UmiCancellationToken *cancel,
                                           UmiLanguageSymbolCatalogue **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (document_uri == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length <= 8192U && document_uri[length] != '\0')
        ++length;
    UmiStatus status = LocationUriValidate(document_uri, length);
    if (status != UMI_STATUS_OK)
        return status;
    UmiJsonTreeLimits limits = {1024U * 1024U, 131072U, 80U};
    UmiJsonTree *tree = NULL;
    status = UmiJsonTreeCreate(json, bytes, &limits, cancel, &tree);
    if (status != UMI_STATUS_OK)
        return status;
    UmiLanguageSymbolCatalogue *catalogue = calloc(1U, sizeof(*catalogue));
    if (catalogue == NULL)
    {
        UmiJsonTreeDestroy(tree);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    catalogue->source_uri = malloc(length + 1U);
    if (catalogue->source_uri == NULL)
        status = UMI_STATUS_OUT_OF_MEMORY;
    else
        memcpy(catalogue->source_uri, document_uri, length + 1U);
    if (status == UMI_STATUS_OK && !UmiJsonTreeIsNull(tree, 0))
    {
        if (UmiJsonTreeKind(tree, 0) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
            status = UMI_STATUS_PARSE_ERROR;
        for (int node = UmiJsonTreeFirst(tree, 0); status == UMI_STATUS_OK && node >= 0;
             node = UmiJsonTreeNext(tree, node))
            status = SymbolRead(catalogue, tree, node, SIZE_MAX, 0U, cancel);
    }
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    UmiJsonTreeDestroy(tree);
    if (status == UMI_STATUS_OK)
        *out = catalogue;
    else
        UmiLanguageSymbolCatalogueDestroy(catalogue);
    return status;
}
UmiStatus UmiLanguageSymbolCatalogueReadResponse(const void *json, size_t bytes, uint64_t expected_id,
                                                 const char *document_uri, const UmiCancellationToken *cancel,
                                                 UmiLanguageSymbolCatalogue **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiJsonTreeLimits limits = {1024U * 1024U, 131072U, 80U};
    UmiJsonTree *tree = NULL;
    int result = -1;
    UmiStatus status = UmiLanguageResponseTreeRead(json, bytes, expected_id, &limits, cancel, &tree, &result);
    const char *value = NULL;
    size_t length = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeSourceSpan(tree, result, &value, &length);
    if (status == UMI_STATUS_OK)
        status = UmiLanguageSymbolCatalogueCreate(value, length, document_uri, cancel, out);
    UmiJsonTreeDestroy(tree);
    return status;
}
void UmiLanguageSymbolCatalogueDestroy(UmiLanguageSymbolCatalogue *catalogue)
{
    if (catalogue == NULL)
        return;
    for (size_t i = 0U; i < catalogue->count; ++i)
    {
        free(catalogue->entries[i].name);
        free(catalogue->entries[i].detail);
        free(catalogue->entries[i].container);
        free(catalogue->entries[i].uri);
    }
    free(catalogue->entries);
    free(catalogue->source_uri);
    free(catalogue);
}
size_t UmiLanguageSymbolCatalogueCount(const UmiLanguageSymbolCatalogue *catalogue)
{
    return catalogue == NULL ? 0U : catalogue->count;
}
UmiStatus UmiLanguageSymbolCatalogueAt(const UmiLanguageSymbolCatalogue *catalogue, size_t index,
                                       UmiLanguageSymbol *out)
{
    if (catalogue == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= catalogue->count)
        return UMI_STATUS_NOT_FOUND;
    *out = catalogue->entries[index].value;
    return UMI_STATUS_OK;
}
