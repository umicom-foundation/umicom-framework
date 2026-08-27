/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/acceptance/evidence.c
 *
 * PURPOSE:
 *   Implement stable acceptance evidence storage keyed by existing Framework developer capability identifiers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/acceptance/evidence.h"
#include <string.h>

void umi_developer_acceptance_evidence_store_init(
    UmiDeveloperAcceptanceEvidenceStore *store)
{
    if (store == NULL) return;
    (void)memset(store, 0, sizeof(*store));
    store->revision = 1U;
}

const UmiDeveloperAcceptanceEvidence *umi_developer_acceptance_evidence_find(
    const UmiDeveloperAcceptanceEvidenceStore *store,
    const char *capability_id)
{
    size_t index;
    if (store == NULL || capability_id == NULL) return NULL;
    for (index = 0U; index < store->count; ++index)
        if (strcmp(store->items[index].capability_id, capability_id) == 0)
            return &store->items[index];
    return NULL;
}

UmiStatus umi_developer_acceptance_evidence_upsert(
    UmiDeveloperAcceptanceEvidenceStore *store,
    const UmiDeveloperAcceptanceEvidence *evidence)
{
    size_t index;
    if (store == NULL || evidence == NULL || evidence->capability_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < store->count; ++index) {
        if (strcmp(store->items[index].capability_id, evidence->capability_id) == 0) {
            store->items[index] = *evidence;
            store->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    if (store->count >= UMI_DEVELOPER_ACCEPTANCE_MAX_EVIDENCE)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    store->items[store->count++] = *evidence;
    store->revision += 1U;
    return UMI_STATUS_OK;
}
