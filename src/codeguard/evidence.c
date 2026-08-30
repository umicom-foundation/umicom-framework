/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/evidence.c
 *
 * PURPOSE:
 *   Implement the evidence behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | CodeGuard evidence store | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/codeguard/evidence.h"
#include <string.h>
const UmiCodeGuardEvidence *umi_codeguard_evidence_find(const UmiCodeGuardEvidenceStore *store,const char *id)
{
    size_t index;
    if (store == NULL || id == NULL) return NULL;
    for (index = 0U; index < store->count; ++index) if (strcmp(store->items[index].id,id) == 0) return &store->items[index];
    return NULL;
}
UmiStatus umi_codeguard_evidence_add(UmiCodeGuardEvidenceStore *store,const UmiCodeGuardEvidence *evidence)
{
    if (store == NULL || evidence == NULL || evidence->id[0] == '\0' || evidence->kind < UMI_CODEGUARD_EVIDENCE_BUILD || evidence->kind > UMI_CODEGUARD_EVIDENCE_DUPLICATION) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_codeguard_evidence_find(store,evidence->id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    if (store->count >= UMI_CODEGUARD_QUALITY_MAX_EVIDENCE) return UMI_STATUS_CAPACITY_EXCEEDED;
    store->items[store->count] = *evidence;
    store->count += 1U;
    store->revision += 1U;
    return UMI_STATUS_OK;
}
size_t umi_codeguard_evidence_count_state(const UmiCodeGuardEvidenceStore *store,UmiCodeGuardEvidenceState state)
{
    size_t index;
    size_t count = 0U;
    if (store == NULL) return 0U;
    for (index = 0U; index < store->count; ++index) if (store->items[index].state == state) count += 1U;
    return count;
}
size_t umi_codeguard_evidence_count_kind(const UmiCodeGuardEvidenceStore *store,UmiCodeGuardEvidenceKind kind)
{
    size_t index;
    size_t count = 0U;
    if (store == NULL) return 0U;
    for (index = 0U; index < store->count; ++index) if (store->items[index].kind == kind) count += 1U;
    return count;
}
