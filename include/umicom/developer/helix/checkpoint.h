/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/checkpoint.h
 *
 * PURPOSE:
 *   Represent a recoverable engineering checkpoint with repository, workspace and validation fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_CHECKPOINT_H
#define UMICOM_DEVELOPER_HELIX_CHECKPOINT_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixCheckpoint {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double confidence;
} UmiHelixCheckpoint;
/* Initialise a deterministic, empty checkpoint record. */
void umi_helix_checkpoint_init(UmiHelixCheckpoint *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_checkpoint_validate(const UmiHelixCheckpoint *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_checkpoint_score(const UmiHelixCheckpoint *record);
#ifdef __cplusplus
}
#endif
#endif
