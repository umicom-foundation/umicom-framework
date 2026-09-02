/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/rollback_checkpoint.h
 *
 * PURPOSE:
 *   Capture the exact pre-change evidence required to restore a governed engineering session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_ROLLBACK_CHECKPOINT_H
#define UMICOM_DEVELOPER_HELIX_ROLLBACK_CHECKPOINT_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the helix rollback checkpoint data shared with callers of this public
 * contract.
 */
typedef struct UmiHelixRollbackCheckpoint {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double confidence;
} UmiHelixRollbackCheckpoint;
/* Initialise a deterministic, empty rollback checkpoint record. */
void umi_helix_rollback_checkpoint_init(UmiHelixRollbackCheckpoint *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_rollback_checkpoint_validate(const UmiHelixRollbackCheckpoint *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_rollback_checkpoint_score(const UmiHelixRollbackCheckpoint *record);
#ifdef __cplusplus
}
#endif
#endif
