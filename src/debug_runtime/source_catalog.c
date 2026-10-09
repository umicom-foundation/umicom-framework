/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/source_catalog.c
 * PURPOSE: Decode bounded source descriptors while preserving optional fields and reference semantics.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/source_catalog.h"
#include "response_fields_internal.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
UmiStatus UmiDebugSourceCatalogDecode(const char *json, UmiDebugSourceCatalog *out)
{
    if (json == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Source paths are copied into an owned catalogue. Keep that bounded but
     * potentially large storage off the application thread's stack. */
    UmiLanguageRuntimeJsonDocument *document = malloc(sizeof *document);
    UmiDebugSourceCatalog *catalog = calloc(1U, sizeof *catalog);
    if (document == NULL || catalog == NULL)
    {
        free(document);
        free(catalog);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    UmiStatus status = umi_language_runtime_json_parse(json, document);
    int body = -1, array = -1;
    if (status == UMI_STATUS_OK)
        status = DebugResponseJsonResponse(document, "loadedSources", 0, &body);
    if (status == UMI_STATUS_OK)
    {
        array = DebugResponseJsonField(document, body, "sources");
        if (array < 0 || document->tokens[array].type != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
            status = UMI_STATUS_PARSE_ERROR;
        else
            catalog->count = umi_language_runtime_json_array_count(document, array);
    }
    if (status == UMI_STATUS_OK && catalog->count > UMI_DEBUG_SOURCE_CATALOG_LIMIT)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < catalog->count; ++i)
    {
        int object = umi_language_runtime_json_array_at(document, array, i);
        UmiDebugSourceRecord *record = &catalog->items[i];
        status = DebugResponseJsonObject(document, object);
#define SOURCE_TEXT(field, key)                                                                    \
    if (status == UMI_STATUS_OK)                                                                   \
    status = DebugResponseJsonString(document, object, key, 0, record->field, sizeof record->field)
        SOURCE_TEXT(name, "name");
        SOURCE_TEXT(path, "path");
        SOURCE_TEXT(origin, "origin");
        SOURCE_TEXT(presentation_hint, "presentationHint");
#undef SOURCE_TEXT
        int reference = DebugResponseJsonField(document, object, "sourceReference");
        if (status == UMI_STATUS_OK && reference >= 0)
        {
            int64_t value = 0;
            status = umi_language_runtime_json_int64(document, reference, &value);
            if (status == UMI_STATUS_OK && (value < 0 || value > INT32_MAX))
                status = UMI_STATUS_PARSE_ERROR;
            if (status == UMI_STATUS_OK)
                record->reference = (uint32_t)value;
        }
        int related = DebugResponseJsonField(document, object, "sources");
        if (status == UMI_STATUS_OK && related >= 0)
        {
            if (document->tokens[related].type != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
                status = UMI_STATUS_PARSE_ERROR;
            else
            {
                record->related_count = umi_language_runtime_json_array_count(document, related);
                for (size_t j = 0U; status == UMI_STATUS_OK && j < record->related_count; ++j)
                    status = DebugResponseJsonObject(
                        document, umi_language_runtime_json_array_at(document, related, j));
            }
        }
        /* The same source may appear under different adapter names or origins.
         * Keep those observations; a source reference is not a unique row key. */
    }
    if (status == UMI_STATUS_OK)
        *out = *catalog;
    free(catalog);
    free(document);
    return status;
}
