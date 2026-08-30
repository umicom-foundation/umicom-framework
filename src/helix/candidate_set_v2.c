/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/candidate_set_v2.c
 * PURPOSE: Implement deterministic Helix v2 candidate comparison.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/helix/candidate_set_v2.h"

#include <stdio.h>
#include <string.h>

static int copy_text(char *destination, size_t capacity, const char *source)
{
    int written = snprintf(destination, capacity, "%s", source);
    return written >= 0 && (size_t)written < capacity;
}

void umi_helix_candidate_set_v2_init(UmiHelixCandidateSetV2 *set)
{
    if (set != NULL) (void)memset(set, 0, sizeof(*set));
}

UmiStatus umi_helix_candidate_set_v2_add(UmiHelixCandidateSetV2 *set,
                                        const char *candidate_id,
                                        double fitness,
                                        int eligible,
                                        const char *provenance_hash)
{
    UmiHelixCandidateScoreV2 *candidate;
    size_t index;
    if (set == NULL || candidate_id == NULL || provenance_hash == NULL ||
        fitness < 0.0 || fitness > 1.0) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < set->count; ++index) {
        if (strcmp(set->candidates[index].candidate_id, candidate_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (set->count >= UMI_HELIX_V2_MAX_CANDIDATES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    candidate = &set->candidates[set->count];
    (void)memset(candidate, 0, sizeof(*candidate));
    if (!copy_text(candidate->candidate_id, sizeof(candidate->candidate_id),
                   candidate_id) ||
        !copy_text(candidate->provenance_hash,
                   sizeof(candidate->provenance_hash), provenance_hash)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    candidate->fitness = fitness;
    candidate->eligible = eligible != 0;
    set->count++;
    return UMI_STATUS_OK;
}

UmiStatus umi_helix_candidate_set_v2_select_best(
    UmiHelixCandidateSetV2 *set,
    UmiHelixCandidateScoreV2 *out_candidate)
{
    size_t index;
    size_t best = SIZE_MAX;
    if (set == NULL || out_candidate == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < set->count; ++index) {
        set->candidates[index].selected = 0;
        if (set->candidates[index].eligible &&
            (best == SIZE_MAX ||
             set->candidates[index].fitness > set->candidates[best].fitness)) {
            best = index;
        }
    }
    if (best == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    set->candidates[best].selected = 1;
    *out_candidate = set->candidates[best];
    return UMI_STATUS_OK;
}
