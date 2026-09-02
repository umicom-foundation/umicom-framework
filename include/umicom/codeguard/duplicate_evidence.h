/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/duplicate_evidence.h
 *
 * PURPOSE:
 *   Publish the public duplicate evidence contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CODEGUARD_DUPLICATE_EVIDENCE_H
#define UMICOM_CODEGUARD_DUPLICATE_EVIDENCE_H
#include "umicom/codeguard/duplicate.h"
#include "umicom/codeguard/quality_types.h"
/**
 * List the named code guard candidate disposition values accepted by this public contract.
 */
typedef enum UmiCodeGuardCandidateDisposition {
    UMI_CODEGUARD_CANDIDATE_REVIEW = 1,
    UMI_CODEGUARD_CANDIDATE_KEEP_SEPARATE,
    UMI_CODEGUARD_CANDIDATE_CONSOLIDATE
} UmiCodeGuardCandidateDisposition;
/**
 * Represent the code guard duplicate candidate data shared with callers of this public
 * contract.
 */
typedef struct UmiCodeGuardDuplicateCandidate {
    char primary_path[UMI_CODEGUARD_QUALITY_PATH_CAPACITY];
    char related_path[UMI_CODEGUARD_QUALITY_PATH_CAPACITY];
    unsigned similarity_percent;
    bool byte_identical;
    bool normalized_identical;
    bool automatic_action_permitted;
    UmiCodeGuardCandidateDisposition disposition;
    char evidence[UMI_CODEGUARD_QUALITY_TEXT_CAPACITY];
} UmiCodeGuardDuplicateCandidate;
/**
 * Represent the code guard duplicate review data shared with callers of this public
 * contract.
 */
typedef struct UmiCodeGuardDuplicateReview {
    UmiCodeGuardDuplicateCandidate items[UMI_CODEGUARD_QUALITY_MAX_CANDIDATES];
    size_t count;
} UmiCodeGuardDuplicateReview;
/**
 * Provide the codeguard duplicate candidate build operation used by this module and its
 * client applications.
 */
UmiStatus umi_codeguard_duplicate_candidate_build(const UmiCodeGuardDuplicateFile *first,const UmiCodeGuardDuplicateFile *second,UmiCodeGuardDuplicateCandidate *out_candidate);
/**
 * Add codeguard duplicate review only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_codeguard_duplicate_review_add(UmiCodeGuardDuplicateReview *review,const UmiCodeGuardDuplicateCandidate *candidate);
/**
 * Provide the codeguard duplicate review disposition operation used by this module and its
 * client applications.
 */
UmiStatus umi_codeguard_duplicate_review_disposition(UmiCodeGuardDuplicateReview *review,size_t index,UmiCodeGuardCandidateDisposition disposition);
#endif
