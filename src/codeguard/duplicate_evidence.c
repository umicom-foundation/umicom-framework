/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/duplicate_evidence.c
 *
 * PURPOSE:
 *   Implement the duplicate evidence behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Evidence-backed duplicate candidates | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/codeguard/duplicate_evidence.h"
#include <stdio.h>
#include <string.h>
UmiStatus umi_codeguard_duplicate_candidate_build(const UmiCodeGuardDuplicateFile *first,const UmiCodeGuardDuplicateFile *second,UmiCodeGuardDuplicateCandidate *out_candidate)
{
    int length;
    if (first == NULL || second == NULL || out_candidate == NULL || first->path[0] == '\0' || second->path[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_candidate,0,sizeof(*out_candidate));
    (void)umi_codeguard_quality_copy(out_candidate->primary_path,sizeof(out_candidate->primary_path),first->path);
    (void)umi_codeguard_quality_copy(out_candidate->related_path,sizeof(out_candidate->related_path),second->path);
    out_candidate->byte_identical = first->raw_hash == second->raw_hash && first->size == second->size && first->size > 0U;
    out_candidate->normalized_identical = first->normalized_hash == second->normalized_hash && first->size > 0U && second->size > 0U;
    out_candidate->similarity_percent = out_candidate->byte_identical ? 100U : (out_candidate->normalized_identical ? 98U : 0U);
    out_candidate->automatic_action_permitted = false;
    out_candidate->disposition = UMI_CODEGUARD_CANDIDATE_REVIEW;
    length = snprintf(out_candidate->evidence,sizeof(out_candidate->evidence),"raw=%s normalized=%s size-a=%zu size-b=%zu; human review required",out_candidate->byte_identical ? "match" : "different",out_candidate->normalized_identical ? "match" : "different",first->size,second->size);
    if (length < 0 || (size_t)length >= sizeof(out_candidate->evidence)) return UMI_STATUS_CAPACITY_EXCEEDED;
    return out_candidate->similarity_percent == 0U ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK;
}
UmiStatus umi_codeguard_duplicate_review_add(UmiCodeGuardDuplicateReview *review,const UmiCodeGuardDuplicateCandidate *candidate)
{
    if (review == NULL || candidate == NULL || candidate->similarity_percent == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (review->count >= UMI_CODEGUARD_QUALITY_MAX_CANDIDATES) return UMI_STATUS_CAPACITY_EXCEEDED;
    review->items[review->count] = *candidate;
    review->items[review->count].automatic_action_permitted = false;
    review->count += 1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_codeguard_duplicate_review_disposition(UmiCodeGuardDuplicateReview *review,size_t index,UmiCodeGuardCandidateDisposition disposition)
{
    if (review == NULL || index >= review->count || disposition < UMI_CODEGUARD_CANDIDATE_REVIEW || disposition > UMI_CODEGUARD_CANDIDATE_CONSOLIDATE) return UMI_STATUS_INVALID_ARGUMENT;
    review->items[index].disposition = disposition;
    review->items[index].automatic_action_permitted = false;
    return UMI_STATUS_OK;
}
