/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/diagnostics_merge.c
 *
 * PURPOSE:
 *   Implement merge diagnostic identities from several providers into one bounded Framework view.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/diagnostics_merge.h"
#include <string.h>
/*
 * Initialise language intelligence diagnostics merge from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_diagnostics_merge_init(UmiLanguageIntelligenceDiagnosticsMerge *catalogue)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL) return;
    (void)memset(catalogue, 0, sizeof(*catalogue));
    catalogue->struct_size = (uint32_t)sizeof(*catalogue);
    catalogue->api_version = UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_MERGE_API_VERSION;
    catalogue->revision = 1U;
}
/*
 * Find language intelligence diagnostics merge while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiLanguageIntelligenceDiagnosticsMergeItem *umi_language_intelligence_diagnostics_merge_find(
    const UmiLanguageIntelligenceDiagnosticsMerge *catalogue,
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
 * Provide the language intelligence diagnostics merge upsert operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_intelligence_diagnostics_merge_upsert(
    UmiLanguageIntelligenceDiagnosticsMerge *catalogue,
    const UmiLanguageIntelligenceDiagnosticsMergeItem *item)
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
 * Provide the language intelligence diagnostics merge best operation used by this module
 * and its client applications.
 */
const UmiLanguageIntelligenceDiagnosticsMergeItem *umi_language_intelligence_diagnostics_merge_best(
    const UmiLanguageIntelligenceDiagnosticsMerge *catalogue)
{
    const UmiLanguageIntelligenceDiagnosticsMergeItem *best = NULL;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        const UmiLanguageIntelligenceDiagnosticsMergeItem *candidate = &catalogue->items[index];
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
