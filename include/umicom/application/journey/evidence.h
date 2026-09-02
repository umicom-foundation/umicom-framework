/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/journey/evidence.h
 *
 * PURPOSE:
 *   Retain bounded, revisioned acceptance evidence for product journey steps.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_JOURNEY_EVIDENCE_H
#define UMICOM_APPLICATION_JOURNEY_EVIDENCE_H
#include "umicom/application/journey/journey.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_APPLICATION_JOURNEY_NOTE_CAPACITY 192U
/**
 * Represent the application journey evidence record data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationJourneyEvidenceRecord {
    char journey_id[96];
    char step_id[96];
    UmiApplicationJourneyResult result;
    char note[UMI_APPLICATION_JOURNEY_NOTE_CAPACITY];
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiApplicationJourneyEvidenceRecord;
/**
 * Represent the application journey evidence registry data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationJourneyEvidenceRegistry {
    UmiApplicationJourneyEvidenceRecord
        records[UMI_APPLICATION_JOURNEY_EVIDENCE_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiApplicationJourneyEvidenceRegistry;
/**
 * Initialise application journey evidence registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_journey_evidence_registry_init(
    UmiApplicationJourneyEvidenceRegistry *registry);
/**
 * Provide the application journey evidence record operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_journey_evidence_record(
    UmiApplicationJourneyEvidenceRegistry *registry,
    const char *journey_id,
    const char *step_id,
    UmiApplicationJourneyResult result,
    const char *note,
    uint64_t timestamp_ms);
/**
 * Find application journey evidence while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiApplicationJourneyEvidenceRecord *
umi_application_journey_evidence_find(
    const UmiApplicationJourneyEvidenceRegistry *registry,
    const char *journey_id,
    const char *step_id);
#ifdef __cplusplus
}
#endif
#endif

