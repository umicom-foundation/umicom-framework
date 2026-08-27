/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/journey/evidence.h
 *
 * PURPOSE:
 *   Retain bounded, revisioned acceptance evidence for product journey steps.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_JOURNEY_EVIDENCE_H
#define UMICOM_APPLICATION_JOURNEY_EVIDENCE_H
#include "umicom/application/journey/journey.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_APPLICATION_JOURNEY_NOTE_CAPACITY 192U
typedef struct UmiApplicationJourneyEvidenceRecord {
    char journey_id[96];
    char step_id[96];
    UmiApplicationJourneyResult result;
    char note[UMI_APPLICATION_JOURNEY_NOTE_CAPACITY];
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiApplicationJourneyEvidenceRecord;
typedef struct UmiApplicationJourneyEvidenceRegistry {
    UmiApplicationJourneyEvidenceRecord
        records[UMI_APPLICATION_JOURNEY_EVIDENCE_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiApplicationJourneyEvidenceRegistry;
void umi_application_journey_evidence_registry_init(
    UmiApplicationJourneyEvidenceRegistry *registry);
UmiStatus umi_application_journey_evidence_record(
    UmiApplicationJourneyEvidenceRegistry *registry,
    const char *journey_id,
    const char *step_id,
    UmiApplicationJourneyResult result,
    const char *note,
    uint64_t timestamp_ms);
const UmiApplicationJourneyEvidenceRecord *
umi_application_journey_evidence_find(
    const UmiApplicationJourneyEvidenceRegistry *registry,
    const char *journey_id,
    const char *step_id);
#ifdef __cplusplus
}
#endif
#endif

