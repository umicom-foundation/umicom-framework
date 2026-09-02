/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/server_trace.c
 *
 * PURPOSE:
 *   Implement maintain bounded language-server trace event identities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/server_trace.h"
#include <string.h>
/*
 * Initialise language intelligence server trace from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_server_trace_init(UmiLanguageIntelligenceServerTrace *catalogue)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL) return;
    (void)memset(catalogue, 0, sizeof(*catalogue));
    catalogue->struct_size = (uint32_t)sizeof(*catalogue);
    catalogue->api_version = UMI_LANGUAGE_INTELLIGENCE_SERVER_TRACE_API_VERSION;
    catalogue->revision = 1U;
}
/*
 * Find language intelligence server trace while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiLanguageIntelligenceServerTraceItem *umi_language_intelligence_server_trace_find(
    const UmiLanguageIntelligenceServerTrace *catalogue,
    const char *id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(catalogue->items[index].id, id) == 0)
            return &catalogue->items[index];
    }
    return NULL;
}
/*
 * Provide the language intelligence server trace upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_intelligence_server_trace_upsert(
    UmiLanguageIntelligenceServerTrace *catalogue,
    const UmiLanguageIntelligenceServerTraceItem *item)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || item == NULL || item->id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(catalogue->items[index].id, item->id) == 0) {
            catalogue->items[index] = *item;
            /* Apply this branch only when its contract condition is satisfied. */
            if (catalogue->revision != UINT64_MAX) catalogue->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (catalogue->count >= UMI_LANGUAGE_INTELLIGENCE_ITEM_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->items[catalogue->count++] = *item;
    /* Apply this branch only when its contract condition is satisfied. */
    if (catalogue->revision != UINT64_MAX) catalogue->revision += 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the language intelligence server trace best operation used by this module and
 * its client applications.
 */
const UmiLanguageIntelligenceServerTraceItem *umi_language_intelligence_server_trace_best(
    const UmiLanguageIntelligenceServerTrace *catalogue)
{
    const UmiLanguageIntelligenceServerTraceItem *best = NULL;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        const UmiLanguageIntelligenceServerTraceItem *candidate = &catalogue->items[index];
        /* Apply this operation only while the related capability or state is available. */
        if (candidate->enabled == 0) continue;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (best == NULL || candidate->priority > best->priority ||
            (candidate->priority == best->priority &&
             candidate->fingerprint > best->fingerprint))
            best = candidate;
    }
    return best;
}
