/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/diagnostic_context.c
 * PURPOSE: Preserve diagnostic identity and opaque server data while selecting source-relevant rows.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/diagnostic_context.h"
#include <stdlib.h>
#include <string.h>
struct UmiLanguageDiagnosticContext
{
    char *json;
    size_t bytes, count;
};
static int ContextCompare(UmiEditorTextPosition a, UmiEditorTextPosition b)
{
    if (a.line != b.line)
        return a.line < b.line ? -1 : 1;
    return a.utf16_column == b.utf16_column ? 0 : a.utf16_column < b.utf16_column ? -1 : 1;
}
static int ContextRelevant(UmiLanguageSourceRange diagnostic, UmiLanguageSourceRange selection)
{
    int point = ContextCompare(diagnostic.start, diagnostic.end) == 0 ||
                ContextCompare(selection.start, selection.end) == 0;
    return point ? (ContextCompare(diagnostic.start, selection.end) <= 0 &&
                    ContextCompare(selection.start, diagnostic.end) <= 0)
                 : (ContextCompare(diagnostic.start, selection.end) < 0 &&
                    ContextCompare(selection.start, diagnostic.end) < 0);
}
UmiStatus UmiLanguageDiagnosticContextCreate(const UmiLanguageDiagnosticCatalogue *catalogue, const char *uri,
                                             const int32_t *version, const char *source, size_t bytes,
                                             size_t start, size_t end, const UmiCancellationToken *cancel,
                                             UmiLanguageDiagnosticContext **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (start > end || end > bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status =
        UmiLanguageDiagnosticCatalogueValidateSource(catalogue, uri, version, source, bytes, cancel);
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = source;
    view.byte_count = bytes;
    view.capacity = bytes;
    UmiLanguageSourceRange selection;
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, start, &selection.start);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, end, &selection.end);
    if (status != UMI_STATUS_OK)
        return status;
    UmiLanguageDiagnosticContext *context = calloc(1U, sizeof(*context));
    if (context == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    size_t count = UmiLanguageDiagnosticCatalogueCount(catalogue);
    /* Measure first, then copy once. The immutable catalogue keeps both passes
     * consistent and a failed allocation never publishes an incomplete array. */
    size_t capacity = 3U;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i)
    {
        if (cancel != NULL && umi_cancellation_token_is_requested(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        UmiLanguageDiagnostic diagnostic;
        status = UmiLanguageDiagnosticCatalogueAt(catalogue, i, &diagnostic);
        if (status == UMI_STATUS_OK && ContextRelevant(diagnostic.range, selection))
        {
            const char *raw = NULL;
            size_t size = 0U;
            status = UmiLanguageDiagnosticCatalogueItemJson(catalogue, i, &raw, &size);
            (void)raw;
            if (status == UMI_STATUS_OK && size + (context->count != 0U) > 1024U * 1024U + 1U - capacity)
                status = UMI_STATUS_CAPACITY_EXCEEDED;
            if (status == UMI_STATUS_OK)
            {
                capacity += size + (context->count != 0U);
                ++context->count;
            }
        }
    }
    if (status == UMI_STATUS_OK)
    {
        context->json = malloc(capacity);
        if (context->json == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
    }
    if (status == UMI_STATUS_OK)
    {
        size_t selected = 0U;
        context->json[context->bytes++] = '[';
        for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i)
        {
            if (cancel != NULL && umi_cancellation_token_is_requested(cancel))
            {
                status = UMI_STATUS_CANCELLED;
                break;
            }
            UmiLanguageDiagnostic diagnostic;
            status = UmiLanguageDiagnosticCatalogueAt(catalogue, i, &diagnostic);
            if (status == UMI_STATUS_OK && ContextRelevant(diagnostic.range, selection))
            {
                const char *raw = NULL;
                size_t size = 0U;
                status = UmiLanguageDiagnosticCatalogueItemJson(catalogue, i, &raw, &size);
                if (status == UMI_STATUS_OK)
                {
                    if (selected++ != 0U)
                        context->json[context->bytes++] = ',';
                    memcpy(context->json + context->bytes, raw, size);
                    context->bytes += size;
                }
            }
        }
        if (status == UMI_STATUS_OK)
        {
            context->json[context->bytes++] = ']';
            context->json[context->bytes] = '\0';
        }
    }
    if (status == UMI_STATUS_OK && cancel != NULL && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        *out = context;
    else
        UmiLanguageDiagnosticContextDestroy(context);
    return status;
}
void UmiLanguageDiagnosticContextDestroy(UmiLanguageDiagnosticContext *context)
{
    if (context != NULL)
    {
        free(context->json);
        free(context);
    }
}
size_t UmiLanguageDiagnosticContextCount(const UmiLanguageDiagnosticContext *context)
{
    return context == NULL ? 0U : context->count;
}
UmiStatus UmiLanguageDiagnosticContextJson(const UmiLanguageDiagnosticContext *context, const char **json,
                                           size_t *bytes)
{
    if (json != NULL)
        *json = NULL;
    if (bytes != NULL)
        *bytes = 0U;
    if (context == NULL || json == NULL || bytes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *json = context->json;
    *bytes = context->bytes;
    return UMI_STATUS_OK;
}
