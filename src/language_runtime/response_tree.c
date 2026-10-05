/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/response_tree.c
 * PURPOSE: Centralise response correlation and error validation for reusable language tools.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/response_tree.h"
#include <string.h>
static UmiStatus ResponseMember(const UmiJsonTree *tree, int object, const char *name, int *node)
{
    *node = -1;
    if (UmiJsonTreeKind(tree, object) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        return UMI_STATUS_PARSE_ERROR;
    UmiStatus status = UmiJsonTreeMember(tree, object, name, node);
    return status == UMI_STATUS_NOT_FOUND ? UMI_STATUS_OK : status;
}
static UmiStatus ResponseText(const UmiJsonTree *tree, int node, char *text, size_t capacity)
{
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_STRING)
        return UMI_STATUS_PARSE_ERROR;
    return UmiJsonTreeText(tree, node, text, capacity);
}
UmiStatus UmiLanguageResponseTreeRead(const void *json, size_t bytes, uint64_t expected_request_id,
                                      const UmiJsonTreeLimits *limits, const UmiCancellationToken *cancel,
                                      UmiJsonTree **out_tree, int *out_result)
{
    if (out_tree != NULL)
        *out_tree = NULL;
    if (out_result != NULL)
        *out_result = -1;
    if (out_tree == NULL || out_result == NULL || json == NULL || bytes == 0U || expected_request_id == 0U ||
        expected_request_id > (uint64_t)INT64_MAX)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiJsonTree *tree = NULL;
    UmiStatus status = UmiJsonTreeCreate(json, bytes, limits, cancel, &tree);
    if (status != UMI_STATUS_OK)
        return status;
    int version, id, method, result, error;
    status = ResponseMember(tree, 0, "jsonrpc", &version);
    if (status == UMI_STATUS_OK)
        status = ResponseMember(tree, 0, "id", &id);
    if (status == UMI_STATUS_OK)
        status = ResponseMember(tree, 0, "method", &method);
    if (status == UMI_STATUS_OK)
        status = ResponseMember(tree, 0, "result", &result);
    if (status == UMI_STATUS_OK)
        status = ResponseMember(tree, 0, "error", &error);
    if (status != UMI_STATUS_OK)
        goto done;
    char protocol[8];
    status = ResponseText(tree, version, protocol, sizeof(protocol));
    if (status != UMI_STATUS_OK)
        goto done;
    if (strcmp(protocol, "2.0") != 0 || method >= 0 || (result < 0) == (error < 0))
    {
        status = UMI_STATUS_PARSE_ERROR;
        goto done;
    }
    int64_t response_id;
    status = UmiJsonTreeInteger(tree, id, &response_id);
    if (status != UMI_STATUS_OK || response_id <= 0)
    {
        status = UMI_STATUS_PARSE_ERROR;
        goto done;
    }
    if ((uint64_t)response_id != expected_request_id)
    {
        status = UMI_STATUS_NOT_FOUND;
        goto done;
    }
    if (error >= 0)
    {
        int code, message;
        int64_t value;
        status = ResponseMember(tree, error, "code", &code);
        if (status == UMI_STATUS_OK)
            status = ResponseMember(tree, error, "message", &message);
        if (status != UMI_STATUS_OK)
            goto done;
        status = UmiJsonTreeInteger(tree, code, &value);
        if (status != UMI_STATUS_OK || value < INT32_MIN || value > INT32_MAX ||
            UmiJsonTreeKind(tree, message) != UMI_LANGUAGE_RUNTIME_JSON_STRING)
        {
            status = UMI_STATUS_PARSE_ERROR;
            goto done;
        }
        status = UMI_STATUS_UNAVAILABLE;
        goto done;
    }

    if (umi_cancellation_token_is_requested(cancel))
    {
        status = UMI_STATUS_CANCELLED;
        goto done;
    }
    *out_result = result;
    *out_tree = tree;
    tree = NULL;
done:
    UmiJsonTreeDestroy(tree);
    return status;
}
