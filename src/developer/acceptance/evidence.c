/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/acceptance/evidence.c
 *
 * PURPOSE:
 *   Implement stable acceptance evidence storage keyed by existing Framework developer capability identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/acceptance/evidence.h"
#include <string.h>

/*
 * Initialise developer acceptance evidence store from caller-provided values so later
 * operations receive a known state.
 */
void umi_developer_acceptance_evidence_store_init(
    UmiDeveloperAcceptanceEvidenceStore *store)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) return;
    (void)memset(store, 0, sizeof(*store));
    store->revision = 1U;
}

/*
 * Find developer acceptance evidence while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiDeveloperAcceptanceEvidence *umi_developer_acceptance_evidence_find(
    const UmiDeveloperAcceptanceEvidenceStore *store,
    const char *capability_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || capability_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < store->count; ++index)
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(store->items[index].capability_id, capability_id) == 0)
            return &store->items[index];
    return NULL;
}

/*
 * Provide the developer acceptance evidence upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_acceptance_evidence_upsert(
    UmiDeveloperAcceptanceEvidenceStore *store,
    const UmiDeveloperAcceptanceEvidence *evidence)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || evidence == NULL || evidence->capability_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < store->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(store->items[index].capability_id, evidence->capability_id) == 0) {
            store->items[index] = *evidence;
            store->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (store->count >= UMI_DEVELOPER_ACCEPTANCE_MAX_EVIDENCE)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    store->items[store->count++] = *evidence;
    store->revision += 1U;
    return UMI_STATUS_OK;
}
