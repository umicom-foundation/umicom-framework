/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/evidence_record.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_EVIDENCE_RECORD_H
#define UMICOM_APPLICATION_PRODUCTION_EVIDENCE_RECORD_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/evidence_requirement.h"

typedef struct UmiApplicationProductionEvidenceRecord {
    char evidence_id[UMI_APPLICATION_PRODUCTION_ID_CAPACITY];
    char reference[UMI_APPLICATION_PRODUCTION_REFERENCE_CAPACITY];
    UmiApplicationProductionEvidenceKind kind;
    UmiApplicationProductionEvidenceState state;
    uint64_t revision;
} UmiApplicationProductionEvidenceRecord;

UmiStatus umi_application_production_evidence_record_set(
    UmiApplicationProductionEvidenceRecord *record,
    const char *evidence_id, UmiApplicationProductionEvidenceKind kind,
    UmiApplicationProductionEvidenceState state, const char *reference,
    uint64_t revision);
UmiStatus umi_application_production_evidence_record_validate(
    const UmiApplicationProductionEvidenceRecord *record);

#ifdef __cplusplus
}
#endif
#endif
