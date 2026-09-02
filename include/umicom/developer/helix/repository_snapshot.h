/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/repository_snapshot.h
 *
 * PURPOSE:
 *   Capture bounded repository health/change evidence at one point in a Helix session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_REPOSITORY_SNAPSHOT_H
#define UMICOM_DEVELOPER_HELIX_REPOSITORY_SNAPSHOT_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the helix repository snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiHelixRepositorySnapshot {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double risk;
} UmiHelixRepositorySnapshot;
/* Initialise a deterministic, empty repository snapshot record. */
void umi_helix_repository_snapshot_init(UmiHelixRepositorySnapshot *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_repository_snapshot_validate(const UmiHelixRepositorySnapshot *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_repository_snapshot_score(const UmiHelixRepositorySnapshot *record);
#ifdef __cplusplus
}
#endif
#endif
