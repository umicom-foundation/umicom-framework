/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/candidate_set.c
 * PURPOSE: Implement deterministic Helix v2 candidate comparison.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/helix/candidate_set.h"

#include <stdio.h>
#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static int copy_text(char *destination, size_t capacity, const char *source)
{
    int written = snprintf(destination, capacity, "%s", source);
    return written >= 0 && (size_t)written < capacity;
}

/*
 * Initialise helix candidate set from caller-provided values so later operations receive a
 * known state.
 */
void umi_helix_candidate_set_init(UmiHelixCandidateSet *set)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set != NULL) (void)memset(set, 0, sizeof(*set));
}

/* Add helix candidate set only after its inputs and available capacity have been checked. */
UmiStatus umi_helix_candidate_set_add(UmiHelixCandidateSet *set,
                                        const char *candidate_id,
                                        double fitness,
                                        int eligible,
                                        const char *provenance_hash)
{
    UmiHelixCandidateScore *candidate;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL || candidate_id == NULL || provenance_hash == NULL ||
        fitness < 0.0 || fitness > 1.0) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < set->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(set->candidates[index].candidate_id, candidate_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (set->count >= UMI_HELIX_RUNTIME_MAX_CANDIDATES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    candidate = &set->candidates[set->count];
    (void)memset(candidate, 0, sizeof(*candidate));
    /* Use the stable identifier comparison to choose the matching record or policy. */
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

/*
 * Provide the helix candidate set select best operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_candidate_set_select_best(
    UmiHelixCandidateSet *set,
    UmiHelixCandidateScore *out_candidate)
{
    size_t index;
    size_t best = SIZE_MAX;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL || out_candidate == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < set->count; ++index) {
        set->candidates[index].selected = 0;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (set->candidates[index].eligible &&
            (best == SIZE_MAX ||
             set->candidates[index].fitness > set->candidates[best].fitness)) {
            best = index;
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (best == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    set->candidates[best].selected = 1;
    *out_candidate = set->candidates[best];
    return UMI_STATUS_OK;
}
