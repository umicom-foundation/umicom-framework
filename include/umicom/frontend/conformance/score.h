/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/score.h
 *
 * PURPOSE:
 *   weighted percentage scoring with blocker-aware grade classification.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef enum UmiFcGrade { UMI_FC_GRADE_A=1,UMI_FC_GRADE_B=2,UMI_FC_GRADE_C=3,UMI_FC_GRADE_FAIL=4 } UmiFcGrade;
double umi_fc_score_weighted(double passed_weight,double total_weight);
UmiFcGrade umi_fc_score_grade(double score,size_t blockers);

#ifdef __cplusplus
}
#endif
#endif
