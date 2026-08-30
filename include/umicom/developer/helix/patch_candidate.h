/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/patch_candidate.h
 *
 * PURPOSE:
 *   Represent one candidate repair or regeneration patch before it is selected for application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_PATCH_CANDIDATE_H
#define UMICOM_DEVELOPER_HELIX_PATCH_CANDIDATE_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixPatchCandidate {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double confidence; double risk; double cost;
} UmiHelixPatchCandidate;
/* Initialise a deterministic, empty patch candidate record. */
void umi_helix_patch_candidate_init(UmiHelixPatchCandidate *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_patch_candidate_validate(const UmiHelixPatchCandidate *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_patch_candidate_score(const UmiHelixPatchCandidate *record);
#ifdef __cplusplus
}
#endif
#endif
