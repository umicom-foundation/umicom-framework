/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/repository_baseline.h
 *
 * PURPOSE:
 *   Capture the trusted repository revision, dirty-state and lock evidence used as the engineering baseline.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_REPOSITORY_BASELINE_H
#define UMICOM_DEVELOPER_HELIX_REPOSITORY_BASELINE_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the helix repository baseline data shared with callers of this public
 * contract.
 */
typedef struct UmiHelixRepositoryBaseline {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double confidence;
} UmiHelixRepositoryBaseline;
/* Initialise a deterministic, empty repository baseline record. */
void umi_helix_repository_baseline_init(UmiHelixRepositoryBaseline *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_repository_baseline_validate(const UmiHelixRepositoryBaseline *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_repository_baseline_score(const UmiHelixRepositoryBaseline *record);
#ifdef __cplusplus
}
#endif
#endif
