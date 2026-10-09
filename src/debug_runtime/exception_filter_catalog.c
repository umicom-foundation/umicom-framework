/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/exception_filter_catalog.c
 * PURPOSE: Decode bounded adapter-defined exception labels without hard-coded language filters.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "exception_filter_json_internal.h"
#include "response_fields_internal.h"
#include <stdlib.h>
#include <string.h>
UmiStatus UmiDebugExceptionCatalogDecode(const char *json, UmiDebugExceptionCatalog *out)
{
    if (json == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* JSON token storage and the catalogue belong on the heap: large adapter
     * responses must not consume the small default Windows thread stack. */
    UmiLanguageRuntimeJsonDocument *document = malloc(sizeof *document);
    UmiDebugExceptionCatalog *catalog = calloc(1U, sizeof *catalog);
    if (document == NULL || catalog == NULL)
    {
        free(document);
        free(catalog);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    UmiStatus status = umi_language_runtime_json_parse(json, document);
    int body = -1, array = -1;
    if (status == UMI_STATUS_OK)
        status = DebugExceptionJsonResponse(document, "initialize", 0, &body);
    if (status == UMI_STATUS_OK)
        array = DebugResponseJsonField(document, body, "exceptionBreakpointFilters");
    if (status == UMI_STATUS_OK && array >= 0)
    {
        if (document->tokens[array].type != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
            status = UMI_STATUS_PARSE_ERROR;
        else
            catalog->count = umi_language_runtime_json_array_count(document, array);
    }
    if (status == UMI_STATUS_OK && catalog->count > UMI_DEBUG_EXCEPTION_FILTER_LIMIT)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < catalog->count; ++i)
    {
        int item = umi_language_runtime_json_array_at(document, array, i);
        UmiDebugExceptionFilter *filter = &catalog->items[i];
        status = DebugExceptionJsonObject(document, item);
        if (status == UMI_STATUS_OK)
            status = DebugExceptionJsonString(document, item, "filter", 1, filter->id,
                                              sizeof filter->id);
        if (status == UMI_STATUS_OK)
            status = DebugExceptionJsonString(document, item, "label", 1, filter->label,
                                              sizeof filter->label);
        if (status == UMI_STATUS_OK && (filter->id[0] == '\0' || filter->label[0] == '\0'))
            status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK)
            status = DebugExceptionJsonString(document, item, "description", 0, filter->description,
                                              sizeof filter->description);
        if (status == UMI_STATUS_OK)
            status =
                DebugExceptionJsonBoolean(document, item, "default", 0, &filter->default_enabled);
        for (size_t j = 0U; status == UMI_STATUS_OK && j < i; ++j)
            if (strcmp(catalog->items[j].id, filter->id) == 0)
                status = UMI_STATUS_PARSE_ERROR;
    }
    if (status == UMI_STATUS_OK)
        *out = *catalog;
    free(catalog);
    free(document);
    return status;
}
