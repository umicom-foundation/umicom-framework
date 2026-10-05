/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/evidence_record.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_EVIDENCE_RECORD_H
#define UMICOM_APPLICATION_PRODUCTION_EVIDENCE_RECORD_H

#include "umicom/application/production/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/evidence_requirement.h"

/**
 * Represent the application production evidence record data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationProductionEvidenceRecord {
    char evidence_id[UMI_APPLICATION_PRODUCTION_ID_CAPACITY];
    char reference[UMI_APPLICATION_PRODUCTION_REFERENCE_CAPACITY];
    UmiApplicationProductionEvidenceKind kind;
    UmiApplicationProductionEvidenceState state;
    uint64_t revision;
} UmiApplicationProductionEvidenceRecord;

/**
 * Copy application production evidence record into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_application_production_evidence_record_set(
    UmiApplicationProductionEvidenceRecord *record,
    const char *evidence_id, UmiApplicationProductionEvidenceKind kind,
    UmiApplicationProductionEvidenceState state, const char *reference,
    uint64_t revision);
/**
 * Check that application production evidence record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_application_production_evidence_record_validate(
    const UmiApplicationProductionEvidenceRecord *record);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_application_production_evidence_record_archive_encode(const UmiApplicationProductionEvidenceRecord *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_application_production_evidence_record_archive_decode(const void *bytes, size_t byte_count,
    UmiApplicationProductionEvidenceRecord *value);

#ifdef __cplusplus
}
#endif
#endif
