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
/* Candidate selection is deterministic: the highest eligible
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

/**
 * Represent the helix candidate score data shared with callers of this public contract.
 */
typedef struct UmiHelixCandidateScore {
    char candidate_id[UMI_HELIX_ID_CAPACITY];
    char provenance_hash[UMI_HELIX_RUNTIME_HASH_CAPACITY];
    double fitness;
    int eligible;
    int selected;
} UmiHelixCandidateScore;

/**
 * Represent the helix candidate set data shared with callers of this public contract.
 */
typedef struct UmiHelixCandidateSet {
    UmiHelixCandidateScore candidates[UMI_HELIX_RUNTIME_MAX_CANDIDATES];
    size_t count;
} UmiHelixCandidateSet;

/**
 * Initialise helix candidate set from caller-provided values so later operations receive a
 * known state.
 */
void umi_helix_candidate_set_init(UmiHelixCandidateSet *set);
/**
 * Add helix candidate set only after its inputs and available capacity have been checked.
 */
UmiStatus umi_helix_candidate_set_add(UmiHelixCandidateSet *set,
                                        const char *candidate_id,
                                        double fitness,
                                        int eligible,
                                        const char *provenance_hash);
/**
 * Provide the helix candidate set select best operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_candidate_set_select_best(
    UmiHelixCandidateSet *set,
    UmiHelixCandidateScore *out_candidate);

#ifdef __cplusplus
}
#endif

#endif
