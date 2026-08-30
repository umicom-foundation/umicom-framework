/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/risk_aggregation.h
 *
 * PURPOSE:
 *   Aggregate independent weighted risk components by root-sum-of-squares.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_RISK_AGGREGATION_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_RISK_AGGREGATION_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Aggregate finite risk components with finite weights using root-sum-of-squares. */
UmiStatus umi_enterprise_risk_aggregate_rss(const double *risks,const double *weights,size_t count,double *out_risk);

#ifdef __cplusplus
}
#endif

#endif
