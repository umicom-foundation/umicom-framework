/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/evidence_registry.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/evidence_registry.h"

#include <string.h>

void umi_application_production_evidence_registry_init(
    UmiApplicationProductionEvidenceRegistry *registry)
{
    if (registry != NULL) {
        (void)memset(registry, 0, sizeof(*registry));
        registry->revision = 1U;
    }
}

const UmiApplicationProductionEvidenceRecord *
umi_application_production_evidence_registry_find(
    const UmiApplicationProductionEvidenceRegistry *registry,
    const char *evidence_id)
{
    size_t index;
    if (registry == NULL || evidence_id == NULL) return NULL;
    for (index = 0U; index < registry->count; ++index)
        if (strcmp(registry->entries[index].evidence_id, evidence_id) == 0)
            return &registry->entries[index];
    return NULL;
}

UmiStatus umi_application_production_evidence_registry_record(
    UmiApplicationProductionEvidenceRegistry *registry,
    const char *evidence_id, UmiApplicationProductionEvidenceKind kind,
    UmiApplicationProductionEvidenceState state, const char *reference)
{
    size_t index;
    UmiApplicationProductionEvidenceRecord record;
    UmiStatus status;
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_production_evidence_record_set(
        &record, evidence_id, kind, state, reference, registry->revision);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->entries[index].evidence_id, evidence_id) == 0) {
            registry->entries[index] = record;
            registry->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    if (registry->count >= UMI_APPLICATION_PRODUCTION_MAX_EVIDENCE)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->entries[registry->count] = record;
    registry->count += 1U;
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

