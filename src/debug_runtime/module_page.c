/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/module_page.c
 * PURPOSE: Decode module metadata without conflating identities, totals or unknown flags.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/module_page.h"
#include "response_fields_internal.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
static UmiStatus ModuleIdentity(const UmiLanguageRuntimeJsonDocument *document, int object,
                                UmiDebugModuleRecord *out)
{
    int token = DebugResponseJsonField(document, object, "id");
    if (token < 0)
        return UMI_STATUS_PARSE_ERROR;
    if (document->tokens[token].type == UMI_LANGUAGE_RUNTIME_JSON_STRING)
    {
        UmiStatus status = UmiLanguageRuntimeJsonText(document, token, out->id, sizeof out->id);
        if (status == UMI_STATUS_OK && out->id[0] == '\0')
            return UMI_STATUS_PARSE_ERROR;
        return status;
    }
    out->numeric_id = 1;
    return umi_language_runtime_json_int64(document, token, &out->number);
}
static UmiStatus ModuleFlag(const UmiLanguageRuntimeJsonDocument *document, int object,
                            const char *key, int *known, int *value)
{
    *known = DebugResponseJsonField(document, object, key) >= 0;
    return DebugResponseJsonBoolean(document, object, key, 0, value);
}
UmiStatus UmiDebugModulePageDecode(const char *json, uint32_t first, uint32_t requested_count,
                                   UmiDebugModulePage *out)
{
    if (json == NULL || out == NULL || first > INT32_MAX || requested_count == 0U ||
        requested_count > UMI_DEBUG_MODULE_PAGE_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* The page contains copied paths and symbol descriptions. Heap ownership
     * avoids placing a large inventory on a native application thread stack. */
    UmiLanguageRuntimeJsonDocument *document = malloc(sizeof *document);
    UmiDebugModulePage *page = calloc(1U, sizeof *page);
    if (document == NULL || page == NULL)
    {
        free(document);
        free(page);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    page->first = first;
    page->requested_count = requested_count;
    UmiStatus status = umi_language_runtime_json_parse(json, document);
    int body = -1, array = -1;
    if (status == UMI_STATUS_OK)
        status = DebugResponseJsonResponse(document, "modules", 0, &body);
    if (status == UMI_STATUS_OK)
    {
        array = DebugResponseJsonField(document, body, "modules");
        if (array < 0 || document->tokens[array].type != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
            status = UMI_STATUS_PARSE_ERROR;
        else
            page->count = umi_language_runtime_json_array_count(document, array);
    }
    if (status == UMI_STATUS_OK && page->count > requested_count)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK)
    {
        /* An absent total is different from zero: a full page may need one more
         * explicit request before the browser can know it reached the end. */
        int total = DebugResponseJsonField(document, body, "totalModules");
        page->total_known = total >= 0;
        if (total >= 0)
        {
            int64_t value = 0;
            status = umi_language_runtime_json_int64(document, total, &value);
            if (status == UMI_STATUS_OK && (value < 0 || value > INT64_C(9007199254740991)))
                status = UMI_STATUS_PARSE_ERROR;
            if (status == UMI_STATUS_OK)
                page->total = (uint64_t)value;
        }
        uint64_t end = (uint64_t)first + page->count;
        if (status == UMI_STATUS_OK && page->total_known &&
            ((page->count != 0U && end > page->total) ||
             (page->count == 0U && page->total > first)))
            status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK)
            page->has_more = page->total_known ? end < page->total : page->count == requested_count;
    }
    for (size_t i = 0U; status == UMI_STATUS_OK && i < page->count; ++i)
    {
        int item = umi_language_runtime_json_array_at(document, array, i);
        UmiDebugModuleRecord *record = &page->items[i];
        status = DebugResponseJsonObject(document, item);
        if (status == UMI_STATUS_OK)
            status = ModuleIdentity(document, item, record);
#define MODULE_TEXT(field, key, required)                                                          \
    if (status == UMI_STATUS_OK)                                                                   \
    status = DebugResponseJsonString(document, item, key, required, record->field,                 \
                                     sizeof record->field)
        MODULE_TEXT(name, "name", 1);
        MODULE_TEXT(path, "path", 0);
        MODULE_TEXT(version, "version", 0);
        MODULE_TEXT(symbol_status, "symbolStatus", 0);
        MODULE_TEXT(symbol_path, "symbolFilePath", 0);
        MODULE_TEXT(timestamp, "dateTimeStamp", 0);
        MODULE_TEXT(address_range, "addressRange", 0);
#undef MODULE_TEXT
        if (status == UMI_STATUS_OK)
            status = ModuleFlag(document, item, "isOptimized", &record->optimized_known,
                                &record->optimized);
        if (status == UMI_STATUS_OK)
            status = ModuleFlag(document, item, "isUserCode", &record->user_code_known,
                                &record->user_code);
        /* An integer ID and a string with the same spelling remain distinct.
         * Repeated identities within one page are ambiguous adapter evidence. */
        for (size_t j = 0U; status == UMI_STATUS_OK && j < i; ++j)
        {
            const UmiDebugModuleRecord *other = &page->items[j];
            if (other->numeric_id == record->numeric_id &&
                (record->numeric_id ? other->number == record->number
                                    : strcmp(other->id, record->id) == 0))
                status = UMI_STATUS_PARSE_ERROR;
        }
    }
    if (status == UMI_STATUS_OK)
        *out = *page;
    free(page);
    free(document);
    return status;
}
