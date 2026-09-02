/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/score.h
 *
 * PURPOSE:
 *   weighted percentage scoring with blocker-aware grade classification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_SCORE_H
#define UMICOM_FRONTEND_CONFORMANCE_SCORE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named fc grade values accepted by this public contract.
 */
typedef enum UmiFcGrade { UMI_FC_GRADE_A=1,UMI_FC_GRADE_B=2,UMI_FC_GRADE_C=3,UMI_FC_GRADE_FAIL=4 } UmiFcGrade;
/**
 * Provide the fc score weighted operation used by this module and its client applications.
 */
double umi_fc_score_weighted(double passed_weight,double total_weight);
/**
 * Provide the fc score grade operation used by this module and its client applications.
 */
UmiFcGrade umi_fc_score_grade(double score,size_t blockers);

#ifdef __cplusplus
}
#endif
#endif
