/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/business_date_adjustment.h
 *
 * PURPOSE:
 *   Define reusable date-adjustment policy metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_BUSINESS_DATE_ADJUSTMENT_H
#define UMICOM_FINANCE_CORE_BUSINESS_DATE_ADJUSTMENT_H

#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiBusinessDateAdjustment { uint32_t flags; uint32_t limit; bool strict; } UmiBusinessDateAdjustment;
/* Initialize conservative defaults. */ void umi_business_date_adjustment_default(UmiBusinessDateAdjustment *p);
/* Evaluate a bounded numeric request. */ bool umi_business_date_adjustment_allows(const UmiBusinessDateAdjustment *p,uint32_t value,uint32_t required_flags);
#ifdef __cplusplus
}
#endif

#endif
