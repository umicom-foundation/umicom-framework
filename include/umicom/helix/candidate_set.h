/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/candidate_set.h
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
#ifndef INCLUDE_UMICOM_HELIX_CANDIDATE_SET_H
#define INCLUDE_UMICOM_HELIX_CANDIDATE_SET_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/helix/types.h"
#include "umicom/helix/execution_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiHelixCandidateScore {
    char candidate_id[UMI_HELIX_ID_CAPACITY];
    char provenance_hash[UMI_HELIX_RUNTIME_HASH_CAPACITY];
    double fitness;
    int eligible;
    int selected;
} UmiHelixCandidateScore;

typedef struct UmiHelixCandidateSet {
    UmiHelixCandidateScore candidates[UMI_HELIX_RUNTIME_MAX_CANDIDATES];
    size_t count;
} UmiHelixCandidateSet;

void umi_helix_candidate_set_init(UmiHelixCandidateSet *set);
UmiStatus umi_helix_candidate_set_add(UmiHelixCandidateSet *set,
                                        const char *candidate_id,
                                        double fitness,
                                        int eligible,
                                        const char *provenance_hash);
UmiStatus umi_helix_candidate_set_select_best(
    UmiHelixCandidateSet *set,
    UmiHelixCandidateScore *out_candidate);

#ifdef __cplusplus
}
#endif

#endif
