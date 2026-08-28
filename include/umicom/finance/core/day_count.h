/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/day_count.h
 *
 * PURPOSE:
 *   Compute reusable day-count fractions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_DAY_COUNT_H
#define UMICOM_FINANCE_CORE_DAY_COUNT_H

#include "umicom/finance/core/business_date.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Compute a supported year fraction. */ UmiStatus umi_day_count_fraction(UmiFinancialDate a,UmiFinancialDate b,UmiDayCountConvention c,double *out);
#ifdef __cplusplus
}
#endif

#endif
