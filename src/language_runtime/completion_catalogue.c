/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/completion_catalogue.c
 * PURPOSE: Keep bounded language-server completion results without losing edit metadata.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "completion_catalogue_internal.h"
#include <stdlib.h>
#include <string.h>

/* A list default is consulted only when the item does not override it.
 * Keep this policy in Framework so every editor interprets the same reply. */
static UmiStatus CompletionInherited(const UmiLanguageCompletionCatalogue *catalogue, int item,
                                     const char *name, uint32_t *value)
{
    int node;
    UmiStatus status = CompletionMember(catalogue->tree, item, name, &node);
    if (status == UMI_STATUS_OK && node < 0)
        status = CompletionMember(catalogue->tree, catalogue->defaults, name, &node);
    if (status == UMI_STATUS_OK && node >= 0)
        status = CompletionInteger(catalogue->tree, node, value);
    return status;
}
UmiStatus UmiLanguageCompletionCatalogueAt(const UmiLanguageCompletionCatalogue *catalogue, size_t index,
                                           UmiLanguageCompletionChoice *out_choice)
{
    if (catalogue == NULL || out_choice == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= catalogue->count)
        return UMI_STATUS_NOT_FOUND;
    UmiLanguageCompletionChoice choice = {0};
    const UmiJsonTree *tree = catalogue->tree;
    int item = catalogue->items[index], node;
    UmiStatus status = CompletionMember(tree, item, "label", &node);
    if (status != UMI_STATUS_OK)
        return status;
    status = CompletionText(tree, node, choice.label, sizeof(choice.label));
    if (status != UMI_STATUS_OK)
        return status;
    memcpy(choice.sort_text, choice.label, strlen(choice.label) + 1U);
    memcpy(choice.filter_text, choice.label, strlen(choice.label) + 1U);
    const char *names[] = {"detail", "sortText", "filterText"};
    char *destinations[] = {choice.detail, choice.sort_text, choice.filter_text};
    const size_t capacities[] = {sizeof(choice.detail), sizeof(choice.sort_text), sizeof(choice.filter_text)};
    for (size_t i = 0U; i < 3U; ++i)
    {
        status = CompletionMember(tree, item, names[i], &node);
        if (status == UMI_STATUS_OK && node >= 0)
            status = CompletionText(tree, node, destinations[i], capacities[i]);
        if (status != UMI_STATUS_OK)
            return status;
    }
    status = CompletionMember(tree, item, "kind", &node);
    if (status == UMI_STATUS_OK && node >= 0)
        status = CompletionInteger(tree, node, &choice.kind);
    if (status != UMI_STATUS_OK)
        return status;
    choice.insert_text_format = 1U;
    choice.insert_text_mode = 1U;
    status = CompletionInherited(catalogue, item, "insertTextFormat", &choice.insert_text_format);
    if (status == UMI_STATUS_OK)
        status = CompletionInherited(catalogue, item, "insertTextMode", &choice.insert_text_mode);
    if (status != UMI_STATUS_OK)
        return status;
    if (choice.insert_text_format < 1U || choice.insert_text_format > 2U || choice.insert_text_mode < 1U ||
        choice.insert_text_mode > 2U)
        return UMI_STATUS_PARSE_ERROR;
    status = CompletionMember(tree, item, "textEdit", &node);
    if (status == UMI_STATUS_OK && node < 0)
        status = CompletionMember(tree, catalogue->defaults, "editRange", &node);
    if (status != UMI_STATUS_OK)
        return status;
    choice.has_text_edit = node >= 0;
    status = CompletionMember(tree, item, "command", &node);
    if (status != UMI_STATUS_OK)
        return status;
    choice.has_command = node >= 0;
    status = CompletionMember(tree, item, "additionalTextEdits", &node);
    if (status != UMI_STATUS_OK)
        return status;
    if (node >= 0)
    {
        if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
            return UMI_STATUS_PARSE_ERROR;
        choice.additional_edit_count = UmiJsonTreeCount(tree, node);
    }
    *out_choice = choice;
    return UMI_STATUS_OK;
}
UmiStatus UmiLanguageCompletionCatalogueCreate(const void *result_json, size_t length,
                                               const UmiCancellationToken *cancel,
                                               UmiLanguageCompletionCatalogue **out_catalogue)
{
    if (out_catalogue == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_catalogue = NULL;
    if (result_json == NULL || length == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiLanguageCompletionCatalogue *catalogue = calloc(1U, sizeof(*catalogue));
    if (catalogue == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    catalogue->defaults = -1;
    UmiJsonTreeLimits limits = {1024U * 1024U, 131072U, 64U};
    UmiStatus status = UmiJsonTreeCreate(result_json, length, &limits, cancel, &catalogue->tree);
    if (status != UMI_STATUS_OK)
        goto failure;
    int array = 0, node;
    if (UmiJsonTreeIsNull(catalogue->tree, 0))
    {
        array = -1;
    }
    else if (UmiJsonTreeKind(catalogue->tree, 0) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
    {
        status = CompletionMember(catalogue->tree, 0, "items", &array);
        if (status != UMI_STATUS_OK)
            goto failure;
        status = CompletionMember(catalogue->tree, 0, "isIncomplete", &node);
        if (status != UMI_STATUS_OK)
            goto failure;
        status = UmiJsonTreeBoolean(catalogue->tree, node, &catalogue->incomplete);
        if (status != UMI_STATUS_OK)
        {
            status = UMI_STATUS_PARSE_ERROR;
            goto failure;
        }
        status = CompletionMember(catalogue->tree, 0, "itemDefaults", &catalogue->defaults);
        if (status != UMI_STATUS_OK)
            goto failure;
        if (catalogue->defaults >= 0 &&
            UmiJsonTreeKind(catalogue->tree, catalogue->defaults) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        {
            status = UMI_STATUS_PARSE_ERROR;
            goto failure;
        }
        /* New merge semantics must be understood before defaults can be used.
         * Explicitly refuse them instead of approximating their behavior. */
        status = CompletionMember(catalogue->tree, 0, "applyKind", &node);
        if (status != UMI_STATUS_OK)
            goto failure;
        if (node >= 0)
        {
            status = UMI_STATUS_NOT_IMPLEMENTED;
            goto failure;
        }
        if (array < 0)
        {
            status = UMI_STATUS_PARSE_ERROR;
            goto failure;
        }
    }
    if (array >= 0)
    {
        if (UmiJsonTreeKind(catalogue->tree, array) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
        {
            status = UMI_STATUS_PARSE_ERROR;
            goto failure;
        }
        catalogue->count = UmiJsonTreeCount(catalogue->tree, array);
        if (catalogue->count > 2048U)
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            goto failure;
        }
        if (catalogue->count > 0U)
        {
            catalogue->items = malloc(catalogue->count * sizeof(*catalogue->items));
            if (catalogue->items == NULL)
            {
                status = UMI_STATUS_OUT_OF_MEMORY;
                goto failure;
            }
        }
        node = UmiJsonTreeFirst(catalogue->tree, array);
        for (size_t i = 0U; i < catalogue->count; ++i)
        {
            if (CompletionCancelled(cancel))
            {
                status = UMI_STATUS_CANCELLED;
                goto failure;
            }
            catalogue->items[i] = node;
            UmiLanguageCompletionChoice choice;
            status = UmiLanguageCompletionCatalogueAt(catalogue, i, &choice);
            if (status != UMI_STATUS_OK)
                goto failure;
            node = UmiJsonTreeNext(catalogue->tree, node);
        }
    }
    if (CompletionCancelled(cancel))
    {
        status = UMI_STATUS_CANCELLED;
        goto failure;
    }
    *out_catalogue = catalogue;
    return UMI_STATUS_OK;
failure:
    UmiLanguageCompletionCatalogueDestroy(catalogue);
    return status;
}
void UmiLanguageCompletionCatalogueDestroy(UmiLanguageCompletionCatalogue *catalogue)
{
    if (catalogue != NULL)
    {
        UmiJsonTreeDestroy(catalogue->tree);
        free(catalogue->items);
        free(catalogue);
    }
}
size_t UmiLanguageCompletionCatalogueCount(const UmiLanguageCompletionCatalogue *catalogue)
{
    return catalogue != NULL ? catalogue->count : 0U;
}
int UmiLanguageCompletionCatalogueIncomplete(const UmiLanguageCompletionCatalogue *catalogue)
{
    return catalogue != NULL && catalogue->incomplete;
}
