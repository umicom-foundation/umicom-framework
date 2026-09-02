/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/provenance_record.h
 *
 * PURPOSE:
 *   Record source revision, model/tool/provider, policy and evidence fingerprints for generated changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_PROVENANCE_RECORD_H
#define UMICOM_DEVELOPER_HELIX_PROVENANCE_RECORD_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the helix provenance record data shared with callers of this public contract.
 */
typedef struct UmiHelixProvenanceRecord {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double confidence;
} UmiHelixProvenanceRecord;
/* Initialise a deterministic, empty provenance record record. */
void umi_helix_provenance_record_init(UmiHelixProvenanceRecord *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_provenance_record_validate(const UmiHelixProvenanceRecord *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_provenance_record_score(const UmiHelixProvenanceRecord *record);
#ifdef __cplusplus
}
#endif
#endif
