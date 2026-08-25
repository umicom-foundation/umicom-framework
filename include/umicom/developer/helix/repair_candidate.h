/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/repair_candidate.h
 *
 * PURPOSE:
 *   Represent one evidence-backed repair proposal with confidence, risk and estimated change size.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_REPAIR_CANDIDATE_H
#define UMICOM_DEVELOPER_HELIX_REPAIR_CANDIDATE_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixRepairCandidate {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double confidence; double risk; double cost;
} UmiHelixRepairCandidate;
/* Initialise a deterministic, empty repair candidate record. */
void umi_helix_repair_candidate_init(UmiHelixRepairCandidate *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_repair_candidate_validate(const UmiHelixRepairCandidate *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_repair_candidate_score(const UmiHelixRepairCandidate *record);
#ifdef __cplusplus
}
#endif
#endif
