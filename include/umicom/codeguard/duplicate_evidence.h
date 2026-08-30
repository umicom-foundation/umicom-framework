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
typedef enum UmiCodeGuardCandidateDisposition {
    UMI_CODEGUARD_CANDIDATE_REVIEW = 1,
    UMI_CODEGUARD_CANDIDATE_KEEP_SEPARATE,
    UMI_CODEGUARD_CANDIDATE_CONSOLIDATE
} UmiCodeGuardCandidateDisposition;
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
typedef struct UmiCodeGuardDuplicateReview {
    UmiCodeGuardDuplicateCandidate items[UMI_CODEGUARD_QUALITY_MAX_CANDIDATES];
    size_t count;
} UmiCodeGuardDuplicateReview;
UmiStatus umi_codeguard_duplicate_candidate_build(const UmiCodeGuardDuplicateFile *first,const UmiCodeGuardDuplicateFile *second,UmiCodeGuardDuplicateCandidate *out_candidate);
UmiStatus umi_codeguard_duplicate_review_add(UmiCodeGuardDuplicateReview *review,const UmiCodeGuardDuplicateCandidate *candidate);
UmiStatus umi_codeguard_duplicate_review_disposition(UmiCodeGuardDuplicateReview *review,size_t index,UmiCodeGuardCandidateDisposition disposition);
#endif
