/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/file_change.h
 *
 * PURPOSE:
 *   Represent a bounded file-level add, modify, delete or rename operation with content fingerprints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_FILE_CHANGE_H
#define UMICOM_DEVELOPER_HELIX_FILE_CHANGE_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixFileChange {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double risk;
} UmiHelixFileChange;
/* Initialise a deterministic, empty file change record. */
void umi_helix_file_change_init(UmiHelixFileChange *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_file_change_validate(const UmiHelixFileChange *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_file_change_score(const UmiHelixFileChange *record);
#ifdef __cplusplus
}
#endif
#endif
