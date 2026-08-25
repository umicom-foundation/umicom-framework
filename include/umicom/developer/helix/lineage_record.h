/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/lineage_record.h
 *
 * PURPOSE:
 *   Record parent/child variant lineage and transformation reason for reproducible evolution history.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_LINEAGE_RECORD_H
#define UMICOM_DEVELOPER_HELIX_LINEAGE_RECORD_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixLineageRecord {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double confidence;
} UmiHelixLineageRecord;
/* Initialise a deterministic, empty lineage record record. */
void umi_helix_lineage_record_init(UmiHelixLineageRecord *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_lineage_record_validate(const UmiHelixLineageRecord *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_lineage_record_score(const UmiHelixLineageRecord *record);
#ifdef __cplusplus
}
#endif
#endif
