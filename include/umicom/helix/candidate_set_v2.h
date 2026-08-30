/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/candidate_set_v2.h
 * PURPOSE: Compare Helix v2 candidates by explicit fitness and provenance.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Candidate selection is deterministic: the highest eligible
 * score wins, and equal scores keep the earliest recorded candidate. */
#ifndef INCLUDE_UMICOM_HELIX_CANDIDATE_SET_V2_H
#define INCLUDE_UMICOM_HELIX_CANDIDATE_SET_V2_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/helix/types.h"
#include "umicom/helix/v2_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiHelixCandidateScoreV2 {
    char candidate_id[UMI_HELIX_ID_CAPACITY];
    char provenance_hash[UMI_HELIX_V2_HASH_CAPACITY];
    double fitness;
    int eligible;
    int selected;
} UmiHelixCandidateScoreV2;

typedef struct UmiHelixCandidateSetV2 {
    UmiHelixCandidateScoreV2 candidates[UMI_HELIX_V2_MAX_CANDIDATES];
    size_t count;
} UmiHelixCandidateSetV2;

void umi_helix_candidate_set_v2_init(UmiHelixCandidateSetV2 *set);
UmiStatus umi_helix_candidate_set_v2_add(UmiHelixCandidateSetV2 *set,
                                        const char *candidate_id,
                                        double fitness,
                                        int eligible,
                                        const char *provenance_hash);
UmiStatus umi_helix_candidate_set_v2_select_best(
    UmiHelixCandidateSetV2 *set,
    UmiHelixCandidateScoreV2 *out_candidate);

#ifdef __cplusplus
}
#endif

#endif
