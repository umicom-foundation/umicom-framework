/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/business_date.h
 *
 * PURPOSE:
 *   Provide stable serial-day arithmetic for financial schedules and settlement.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_BUSINESS_DATE_H
#define UMICOM_FINANCE_CORE_BUSINESS_DATE_H

#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Convert a valid date to serial days relative to 1970-01-01. */ UmiStatus umi_business_date_serial(UmiFinancialDate d,int64_t *out);
/* Convert serial days back to a valid date. */ UmiStatus umi_business_date_from_serial(int64_t serial,UmiFinancialDate *out);
/* Add calendar days. */ UmiStatus umi_business_date_add(UmiFinancialDate d,int32_t days,UmiFinancialDate *out);
/* Return Monday=1 through Sunday=7. */ UmiStatus umi_business_date_weekday(UmiFinancialDate d,uint8_t *out);
#ifdef __cplusplus
}
#endif

#endif
