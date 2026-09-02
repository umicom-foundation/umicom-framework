/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/autonomy_level.h
 *
 * PURPOSE:
 *   Classify manual, assisted, supervised and guarded-autonomous operating modes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_AUTONOMY_LEVEL_H
#define UMICOM_DEVELOPER_HELIX_AUTONOMY_LEVEL_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the helix autonomy level data shared with callers of this public contract.
 */
typedef struct UmiHelixAutonomyLevel {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double progress;
} UmiHelixAutonomyLevel;
/* Initialise a deterministic, empty autonomy level record. */
void umi_helix_autonomy_level_init(UmiHelixAutonomyLevel *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_autonomy_level_validate(const UmiHelixAutonomyLevel *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_autonomy_level_score(const UmiHelixAutonomyLevel *record);
#ifdef __cplusplus
}
#endif
#endif
