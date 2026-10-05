/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/completion_response.c
 * PURPOSE: Correlate a completion response before exposing its owned result.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "completion_catalogue_internal.h"
#include <string.h>

#include "umicom/language_runtime/response_tree.h"

/* Shared response-tree validation replaces the completion-specific copy.
 * Retain that implementation for review of correlation and error behavior. */
#if 0
UmiStatus UmiLanguageCompletionCatalogueReadResponse(const void *response_json, size_t length,
                                                     uint64_t expected_request_id,
                                                     const UmiCancellationToken *cancel,
                                                     UmiLanguageCompletionCatalogue **out_catalogue)
{
    if (out_catalogue == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_catalogue = NULL;
    if (response_json == NULL || length == 0U || expected_request_id == 0U ||
        expected_request_id > (uint64_t)INT64_MAX)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiJsonTreeLimits limits = {1024U * 1024U, 131072U, 64U};
    UmiJsonTree *tree = NULL;
    UmiStatus status = UmiJsonTreeCreate(response_json, length, &limits, cancel, &tree);
    if (status != UMI_STATUS_OK)
        return status;
    int version, id, method, result, error;
    status = CompletionMember(tree, 0, "jsonrpc", &version);
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, 0, "id", &id);
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, 0, "method", &method);
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, 0, "result", &result);
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, 0, "error", &error);
    if (status != UMI_STATUS_OK)
        goto done;
    char protocol[8];
    status = CompletionText(tree, version, protocol, sizeof(protocol));
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
        status = CompletionMember(tree, error, "code", &code);
        if (status == UMI_STATUS_OK)
            status = CompletionMember(tree, error, "message", &message);
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
    const char *bytes;
    size_t count;
    status = UmiJsonTreeSourceSpan(tree, result, &bytes, &count);
    if (status == UMI_STATUS_OK)
        status = UmiLanguageCompletionCatalogueCreate(bytes, count, cancel, out_catalogue);
done:
    UmiJsonTreeDestroy(tree);
    return status;
}

#endif

/* Response correlation is shared with formatting and other source tools so
 * malformed envelopes cannot bypass a tool-specific reader. The previous
 * completion-only validator is retained above for engineering review. */
UmiStatus UmiLanguageCompletionCatalogueReadResponse(const void *response_json, size_t length,
                                                     uint64_t expected_request_id,
                                                     const UmiCancellationToken *cancel,
                                                     UmiLanguageCompletionCatalogue **out_catalogue)
{
    if (out_catalogue == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_catalogue = NULL;
    UmiJsonTreeLimits limits = {1024U * 1024U, 131072U, 64U};
    UmiJsonTree *tree = NULL;
    int result = -1;
    UmiStatus status = UmiLanguageResponseTreeRead(response_json, length, expected_request_id, &limits,
                                                   cancel, &tree, &result);
    const char *bytes = NULL;
    size_t count = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeSourceSpan(tree, result, &bytes, &count);
    if (status == UMI_STATUS_OK)
        status = UmiLanguageCompletionCatalogueCreate(bytes, count, cancel, out_catalogue);
    UmiJsonTreeDestroy(tree);
    return status;
}
