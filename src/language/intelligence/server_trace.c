/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/server_trace.c
 *
 * PURPOSE:
 *   Implement maintain bounded language-server trace event identities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/server_trace.h"
#include <string.h>
void umi_language_intelligence_server_trace_init(UmiLanguageIntelligenceServerTrace *catalogue)
{
    if (catalogue == NULL) return;
    (void)memset(catalogue, 0, sizeof(*catalogue));
    catalogue->struct_size = (uint32_t)sizeof(*catalogue);
    catalogue->api_version = UMI_LANGUAGE_INTELLIGENCE_SERVER_TRACE_API_VERSION;
    catalogue->revision = 1U;
}
const UmiLanguageIntelligenceServerTraceItem *umi_language_intelligence_server_trace_find(
    const UmiLanguageIntelligenceServerTrace *catalogue,
    const char *id)
{
    size_t index;
    if (catalogue == NULL || id == NULL) return NULL;
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->items[index].id, id) == 0)
            return &catalogue->items[index];
    }
    return NULL;
}
UmiStatus umi_language_intelligence_server_trace_upsert(
    UmiLanguageIntelligenceServerTrace *catalogue,
    const UmiLanguageIntelligenceServerTraceItem *item)
{
    size_t index;
    if (catalogue == NULL || item == NULL || item->id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->items[index].id, item->id) == 0) {
            catalogue->items[index] = *item;
            if (catalogue->revision != UINT64_MAX) catalogue->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    if (catalogue->count >= UMI_LANGUAGE_INTELLIGENCE_ITEM_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->items[catalogue->count++] = *item;
    if (catalogue->revision != UINT64_MAX) catalogue->revision += 1U;
    return UMI_STATUS_OK;
}
const UmiLanguageIntelligenceServerTraceItem *umi_language_intelligence_server_trace_best(
    const UmiLanguageIntelligenceServerTrace *catalogue)
{
    const UmiLanguageIntelligenceServerTraceItem *best = NULL;
    size_t index;
    if (catalogue == NULL) return NULL;
    for (index = 0U; index < catalogue->count; ++index) {
        const UmiLanguageIntelligenceServerTraceItem *candidate = &catalogue->items[index];
        if (candidate->enabled == 0) continue;
        if (best == NULL || candidate->priority > best->priority ||
            (candidate->priority == best->priority &&
             candidate->fingerprint > best->fingerprint))
            best = candidate;
    }
    return best;
}
