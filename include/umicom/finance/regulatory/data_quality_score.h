/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/data_quality_score.h
 *
 * PURPOSE:
 *   Aggregate weighted data-quality dimensions into one bounded score.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_DATA_QUALITY_SCORE_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_DATA_QUALITY_SCORE_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Compute a weighted average quality score from finite [0,1] observations. */
UmiStatus umi_reg_data_quality_score_compute(const double *scores,const double *weights,size_t count,double *out_score);

#ifdef __cplusplus
}
#endif

#endif
