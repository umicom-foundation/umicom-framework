/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/evidence.c
 *
 * PURPOSE:
 *   Record bounded journey evidence with deterministic replacement semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/journey/evidence.h"
#include <stdio.h>
#include <string.h>
void umi_application_journey_evidence_registry_init(
    UmiApplicationJourneyEvidenceRegistry *registry)
{
    if (registry != NULL) (void)memset(registry, 0, sizeof(*registry));
}
const UmiApplicationJourneyEvidenceRecord *
umi_application_journey_evidence_find(
    const UmiApplicationJourneyEvidenceRegistry *registry,
    const char *journey_id,
    const char *step_id)
{
    size_t index;
    if (registry == NULL || journey_id == NULL || step_id == NULL) return NULL;
    for (index = 0U; index < registry->count; ++index)
        if (strcmp(registry->records[index].journey_id, journey_id) == 0 &&
            strcmp(registry->records[index].step_id, step_id) == 0)
            return &registry->records[index];
    return NULL;
}
UmiStatus umi_application_journey_evidence_record(
    UmiApplicationJourneyEvidenceRegistry *registry,
    const char *journey_id,
    const char *step_id,
    UmiApplicationJourneyResult result,
    const char *note,
    uint64_t timestamp_ms)
{
    size_t index;
    UmiApplicationJourneyEvidenceRecord *record = NULL;
    if (registry == NULL || journey_id == NULL || journey_id[0] == '\0' ||
        step_id == NULL || step_id[0] == '\0' ||
        result < UMI_APPLICATION_JOURNEY_PENDING ||
        result > UMI_APPLICATION_JOURNEY_BLOCKED)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < registry->count; ++index)
        if (strcmp(registry->records[index].journey_id, journey_id) == 0 &&
            strcmp(registry->records[index].step_id, step_id) == 0) {
            record = &registry->records[index];
            break;
        }
    if (record == NULL) {
        if (registry->count >= UMI_APPLICATION_JOURNEY_EVIDENCE_CAPACITY)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        record = &registry->records[registry->count++];
        (void)memset(record, 0, sizeof(*record));
        (void)snprintf(record->journey_id, sizeof(record->journey_id), "%s",
                       journey_id);
        (void)snprintf(record->step_id, sizeof(record->step_id), "%s", step_id);
    }
    registry->revision++;
    record->result = result;
    record->timestamp_ms = timestamp_ms;
    record->revision = registry->revision;
    (void)snprintf(record->note, sizeof(record->note), "%s",
                   note != NULL ? note : "");
    return UMI_STATUS_OK;
}

