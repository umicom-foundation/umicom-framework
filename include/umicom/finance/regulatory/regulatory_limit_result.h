/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/regulatory_limit_result.h
 *
 * PURPOSE:
 *   Evaluate observed regulatory metrics against minimum or maximum limits.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_REGULATORY_LIMIT_RESULT_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_REGULATORY_LIMIT_RESULT_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiRegulatoryLimitResult { char limit_id[UMI_REG_ID_CAPACITY]; double observed; double threshold; int breached; } UmiRegulatoryLimitResult;
/* Evaluate one observed metric against an inclusive minimum or maximum threshold. */
UmiStatus umi_reg_regulatory_limit_result_evaluate(UmiRegulatoryLimitResult *result,const char *limit_id,double observed,double threshold,int minimum_rule);

#ifdef __cplusplus
}
#endif

#endif
