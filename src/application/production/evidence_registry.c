/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/evidence_registry.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/evidence_registry.h"

#include <string.h>

/*
 * Initialise application production evidence registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_production_evidence_registry_init(
    UmiApplicationProductionEvidenceRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry != NULL) {
        (void)memset(registry, 0, sizeof(*registry));
        registry->revision = 1U;
    }
}

/*
 * Find application production evidence registry while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationProductionEvidenceRecord *
umi_application_production_evidence_registry_find(
    const UmiApplicationProductionEvidenceRegistry *registry,
    const char *evidence_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || evidence_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index)
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->entries[index].evidence_id, evidence_id) == 0)
            return &registry->entries[index];
    return NULL;
}

/*
 * Provide the application production evidence registry record operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_production_evidence_registry_record(
    UmiApplicationProductionEvidenceRegistry *registry,
    const char *evidence_id, UmiApplicationProductionEvidenceKind kind,
    UmiApplicationProductionEvidenceState state, const char *reference)
{
    size_t index;
    UmiApplicationProductionEvidenceRecord record;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_production_evidence_record_set(
        &record, evidence_id, kind, state, reference, registry->revision);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->entries[index].evidence_id, evidence_id) == 0) {
            registry->entries[index] = record;
            registry->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_APPLICATION_PRODUCTION_MAX_EVIDENCE)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->entries[registry->count] = record;
    registry->count += 1U;
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

