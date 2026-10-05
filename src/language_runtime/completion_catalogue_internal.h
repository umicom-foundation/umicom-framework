/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/completion_catalogue_internal.h
 * PURPOSE: Share owned completion JSON between catalogue and editor planning.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_COMPLETION_CATALOGUE_INTERNAL_H
#define UMICOM_COMPLETION_CATALOGUE_INTERNAL_H
#include "umicom/language_runtime/completion_catalogue.h"
#include "umicom/language_runtime/json_tree.h"
struct UmiLanguageCompletionCatalogue
{
    UmiJsonTree *tree;
    int *items;
    int defaults, incomplete;
    size_t count;
};
/* Optional lookup distinguishes absence from malformed or duplicate members.
 * Helpers return PARSE_ERROR for wrong JSON shapes, keeping caller errors
 * distinct from a server response that cannot be interpreted safely. */
static inline UmiStatus CompletionMember(const UmiJsonTree *tree, int object, const char *name, int *node)
{
    *node = -1;
    if (object < 0)
        return UMI_STATUS_OK;
    if (UmiJsonTreeKind(tree, object) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        return UMI_STATUS_PARSE_ERROR;
    UmiStatus status = UmiJsonTreeMember(tree, object, name, node);
    return status == UMI_STATUS_NOT_FOUND ? UMI_STATUS_OK : status;
}
static inline UmiStatus CompletionText(const UmiJsonTree *tree, int node, char *text, size_t capacity)
{
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_STRING)
        return UMI_STATUS_PARSE_ERROR;
    return UmiJsonTreeText(tree, node, text, capacity);
}
static inline UmiStatus CompletionInteger(const UmiJsonTree *tree, int node, uint32_t *value)
{
    int64_t integer;
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_PRIMITIVE)
        return UMI_STATUS_PARSE_ERROR;
    UmiStatus status = UmiJsonTreeInteger(tree, node, &integer);
    if (status != UMI_STATUS_OK)
        return status;
    if (integer < 0 || integer > INT32_MAX)
        return UMI_STATUS_PARSE_ERROR;
    *value = (uint32_t)integer;
    return UMI_STATUS_OK;
}
static inline int CompletionCancelled(const UmiCancellationToken *cancel)
{
    return cancel != NULL && umi_cancellation_token_is_requested(cancel);
}
#endif
