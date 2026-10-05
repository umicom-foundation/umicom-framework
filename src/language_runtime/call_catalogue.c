/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/call_catalogue.c
 * PURPOSE: Validate and retain complete call relationships without crossing source ownership or server-session boundaries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/call_catalogue.h"
#include "umicom/language_runtime/response_tree.h"
#include "symbol_text_internal.h"
typedef struct CallItemEntry
{
    UmiLanguageCallItem value;
    char *name, *detail, *uri;
    int node;
} CallItemEntry;
struct UmiLanguageCallItemCatalogue
{
    UmiJsonTree *tree;
    CallItemEntry *items;
    size_t count;
};
typedef struct CallEdgeEntry
{
    CallItemEntry item;
    UmiLanguageSourceRange *sites;
    size_t count;
} CallEdgeEntry;
struct UmiLanguageCallEdgeCatalogue
{
    CallEdgeEntry *edges;
    size_t count;
    char *root_uri;
    UmiLanguageCallDirection direction;
};
static void CallItemClear(CallItemEntry *entry)
{
    free(entry->name);
    free(entry->detail);
    free(entry->uri);
}
static UmiStatus CallItemRead(const UmiJsonTree *tree, int node, CallItemEntry *entry,
                              const UmiCancellationToken *cancel)
{
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    entry->node = node;
    UmiStatus status = SymbolText(tree, node, "name", 1, &entry->name);
    if (status == UMI_STATUS_OK && !SymbolVisibleName(entry->name))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = SymbolText(tree, node, "detail", 0, &entry->detail);
    int kind = -1, uri = -1, range = -1, selection = -1, tags = -1, data = -1;
    int64_t integer = 0;
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, node, "kind", 1, &kind);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeInteger(tree, kind, &integer);
    if (status == UMI_STATUS_OK && (integer < 1 || integer > INT32_MAX))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        entry->value.kind = (int32_t)integer;
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, node, "uri", 1, &uri);
    if (status == UMI_STATUS_OK)
        status = LocationUri(tree, uri, &entry->uri);
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, node, "range", 1, &range);
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, node, "selectionRange", 1, &selection);
    if (status == UMI_STATUS_OK)
        status = LocationRange(tree, range, &entry->value.location.target);
    if (status == UMI_STATUS_OK)
        status = LocationRange(tree, selection, &entry->value.location.selection);
    if (status == UMI_STATUS_OK &&
        (LocationCompare(entry->value.location.selection.start, entry->value.location.target.start) < 0 ||
         LocationCompare(entry->value.location.selection.end, entry->value.location.target.end) > 0))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, node, "data", 0, &data);
    entry->value.has_data = data >= 0;
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, node, "tags", 0, &tags);
    if (status == UMI_STATUS_OK && tags >= 0)
    {
        if (UmiJsonTreeKind(tree, tags) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
            status = UMI_STATUS_PARSE_ERROR;
        for (int tag = UmiJsonTreeFirst(tree, tags); status == UMI_STATUS_OK && tag >= 0;
             tag = UmiJsonTreeNext(tree, tag))
        {
            if (umi_cancellation_token_is_requested(cancel))
                return UMI_STATUS_CANCELLED;
            status = UmiJsonTreeInteger(tree, tag, &integer);
            if (status == UMI_STATUS_OK && (integer < 1 || integer > INT32_MAX))
                status = UMI_STATUS_PARSE_ERROR;
            if (status == UMI_STATUS_OK && integer == 1)
                entry->value.deprecated = 1;
        }
    }
    entry->value.name = entry->name;
    entry->value.detail = entry->detail == NULL ? "" : entry->detail;
    entry->value.location.uri = entry->uri;
    return status;
}
UmiStatus UmiLanguageCallItemCatalogueCreate(const void *json, size_t bytes,
                                             const UmiCancellationToken *cancel,
                                             UmiLanguageCallItemCatalogue **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiLanguageCallItemCatalogue *owner = calloc(1U, sizeof(*owner));
    if (owner == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiJsonTreeLimits limits = {1024U * 1024U, 131072U, 80U};
    UmiStatus status = UmiJsonTreeCreate(json, bytes, &limits, cancel, &owner->tree);
    if (status == UMI_STATUS_OK && !UmiJsonTreeIsNull(owner->tree, 0))
    {
        if (UmiJsonTreeKind(owner->tree, 0) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
            status = UMI_STATUS_PARSE_ERROR;
        size_t count = status == UMI_STATUS_OK ? UmiJsonTreeCount(owner->tree, 0) : 0U;
        if (count > 4096U)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        if (status == UMI_STATUS_OK && count != 0U)
        {
            owner->items = calloc(count, sizeof(*owner->items));
            if (owner->items == NULL)
                status = UMI_STATUS_OUT_OF_MEMORY;
        }
        /* Count each allocated entry before reading it, so destruction also
         * releases partially decoded text when a later field is malformed. */
        for (int node = UmiJsonTreeFirst(owner->tree, 0); status == UMI_STATUS_OK && node >= 0;
             node = UmiJsonTreeNext(owner->tree, node))
            status = CallItemRead(owner->tree, node, &owner->items[owner->count++], cancel);
    }
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        *out = owner;
    else
        UmiLanguageCallItemCatalogueDestroy(owner);
    return status;
}
UmiStatus UmiLanguageCallItemCatalogueReadResponse(const void *json, size_t bytes, uint64_t id,
                                                   const UmiCancellationToken *cancel,
                                                   UmiLanguageCallItemCatalogue **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiJsonTree *tree = NULL;
    int result = -1;
    const char *span = NULL;
    size_t length = 0U;
    UmiJsonTreeLimits limits = {1024U * 1024U, 131072U, 80U};
    UmiStatus status = UmiLanguageResponseTreeRead(json, bytes, id, &limits, cancel, &tree, &result);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeSourceSpan(tree, result, &span, &length);
    if (status == UMI_STATUS_OK)
        status = UmiLanguageCallItemCatalogueCreate(span, length, cancel, out);
    UmiJsonTreeDestroy(tree);
    return status;
}
void UmiLanguageCallItemCatalogueDestroy(UmiLanguageCallItemCatalogue *owner)
{
    if (owner == NULL)
        return;
    for (size_t i = 0U; i < owner->count; ++i)
        CallItemClear(&owner->items[i]);
    free(owner->items);
    UmiJsonTreeDestroy(owner->tree);
    free(owner);
}
size_t UmiLanguageCallItemCatalogueCount(const UmiLanguageCallItemCatalogue *owner)
{
    return owner == NULL ? 0U : owner->count;
}
UmiStatus UmiLanguageCallItemCatalogueAt(const UmiLanguageCallItemCatalogue *owner, size_t index,
                                         UmiLanguageCallItem *out)
{
    if (owner == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= owner->count)
        return UMI_STATUS_NOT_FOUND;
    *out = owner->items[index].value;
    return UMI_STATUS_OK;
}
UmiStatus UmiLanguageCallItemCatalogueItemJson(const UmiLanguageCallItemCatalogue *owner, size_t index,
                                               const char **out_json, size_t *out_bytes)
{
    if (owner == NULL || out_json == NULL || out_bytes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= owner->count)
        return UMI_STATUS_NOT_FOUND;
    return UmiJsonTreeSourceSpan(owner->tree, owner->items[index].node, out_json, out_bytes);
}
UmiStatus UmiLanguageCallEdgeCatalogueReadResponse(const void *json, size_t bytes, uint64_t id,
                                                   const UmiLanguageCallItem *root,
                                                   UmiLanguageCallDirection direction,
                                                   const UmiCancellationToken *cancel,
                                                   UmiLanguageCallEdgeCatalogue **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (root == NULL || root->location.uri == NULL ||
        (direction != UMI_LANGUAGE_CALL_INCOMING && direction != UMI_LANGUAGE_CALL_OUTGOING))
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length <= 8192U && root->location.uri[length] != '\0')
        ++length;
    UmiStatus status = LocationUriValidate(root->location.uri, length);
    if (status != UMI_STATUS_OK)
        return status;
    UmiLanguageCallEdgeCatalogue *owner = calloc(1U, sizeof(*owner));
    if (owner == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    owner->direction = direction;
    owner->root_uri = malloc(length + 1U);
    if (owner->root_uri == NULL)
        status = UMI_STATUS_OUT_OF_MEMORY;
    else
        memcpy(owner->root_uri, root->location.uri, length + 1U);
    UmiJsonTree *tree = NULL;
    int result = -1;
    UmiJsonTreeLimits limits = {1024U * 1024U, 131072U, 80U};
    if (status == UMI_STATUS_OK)
        status = UmiLanguageResponseTreeRead(json, bytes, id, &limits, cancel, &tree, &result);
    if (status == UMI_STATUS_OK && !UmiJsonTreeIsNull(tree, result))
    {
        if (UmiJsonTreeKind(tree, result) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
            status = UMI_STATUS_PARSE_ERROR;
        size_t count = status == UMI_STATUS_OK ? UmiJsonTreeCount(tree, result) : 0U, total = 0U;
        if (count > 4096U)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        if (status == UMI_STATUS_OK && count != 0U)
        {
            owner->edges = calloc(count, sizeof(*owner->edges));
            if (owner->edges == NULL)
                status = UMI_STATUS_OUT_OF_MEMORY;
        }
        for (int node = UmiJsonTreeFirst(tree, result); status == UMI_STATUS_OK && node >= 0;
             node = UmiJsonTreeNext(tree, node))
        {
            if (umi_cancellation_token_is_requested(cancel))
            {
                status = UMI_STATUS_CANCELLED;
                break;
            }
            CallEdgeEntry *edge = &owner->edges[owner->count++];
            int item = -1, ranges = -1;
            status =
                LocationMember(tree, node, direction == UMI_LANGUAGE_CALL_INCOMING ? "from" : "to", 1, &item);
            if (status == UMI_STATUS_OK)
                status = CallItemRead(tree, item, &edge->item, cancel);
            if (status == UMI_STATUS_OK)
                status = LocationMember(tree, node, "fromRanges", 1, &ranges);
            if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, ranges) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
                status = UMI_STATUS_PARSE_ERROR;
            size_t sites = status == UMI_STATUS_OK ? UmiJsonTreeCount(tree, ranges) : 0U;
            if (sites > 16384U - total)
                status = UMI_STATUS_CAPACITY_EXCEEDED;
            if (status == UMI_STATUS_OK && sites != 0U)
            {
                edge->sites = calloc(sites, sizeof(*edge->sites));
                if (edge->sites == NULL)
                    status = UMI_STATUS_OUT_OF_MEMORY;
            }
            for (int range = UmiJsonTreeFirst(tree, ranges); status == UMI_STATUS_OK && range >= 0;
                 range = UmiJsonTreeNext(tree, range))
            {
                if (umi_cancellation_token_is_requested(cancel))
                {
                    status = UMI_STATUS_CANCELLED;
                    break;
                }
                status = LocationRange(tree, range, &edge->sites[edge->count++]);
            }
            total += sites;
        }
    }
    UmiJsonTreeDestroy(tree);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        *out = owner;
    else
        UmiLanguageCallEdgeCatalogueDestroy(owner);
    return status;
}
void UmiLanguageCallEdgeCatalogueDestroy(UmiLanguageCallEdgeCatalogue *owner)
{
    if (owner == NULL)
        return;
    for (size_t i = 0U; i < owner->count; ++i)
    {
        CallItemClear(&owner->edges[i].item);
        free(owner->edges[i].sites);
    }
    free(owner->edges);
    free(owner->root_uri);
    free(owner);
}
size_t UmiLanguageCallEdgeCatalogueCount(const UmiLanguageCallEdgeCatalogue *owner)
{
    return owner == NULL ? 0U : owner->count;
}
UmiStatus UmiLanguageCallEdgeCatalogueAt(const UmiLanguageCallEdgeCatalogue *owner, size_t index,
                                         UmiLanguageCallEdge *out)
{
    if (owner == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= owner->count)
        return UMI_STATUS_NOT_FOUND;
    *out = (UmiLanguageCallEdge){owner->edges[index].item.value, owner->edges[index].count};
    return UMI_STATUS_OK;
}
UmiStatus UmiLanguageCallEdgeCatalogueLocation(const UmiLanguageCallEdgeCatalogue *owner, size_t index,
                                               size_t destination, UmiLanguageSourceLocation *out)
{
    if (owner == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= owner->count)
        return UMI_STATUS_NOT_FOUND;
    const CallEdgeEntry *edge = &owner->edges[index];
    if (destination > edge->count)
        return UMI_STATUS_NOT_FOUND;
    if (destination == 0U)
        *out = edge->item.value.location;
    else
    {
        /* A callee's symbol URI does not own the outgoing call expression.
         * Keep the caller URI even when the destination lives in another file. */
        const char *uri =
            owner->direction == UMI_LANGUAGE_CALL_INCOMING ? edge->item.value.location.uri : owner->root_uri;
        *out = (UmiLanguageSourceLocation){
            .uri = uri, .target = edge->sites[destination - 1U], .selection = edge->sites[destination - 1U]};
    }
    return UMI_STATUS_OK;
}
