/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/pricing_context.h
 *
 * PURPOSE:
 *   Define pricing context identity and valuation date.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_PRICING_CONTEXT_H
#define UMICOM_FINANCE_CORE_PRICING_CONTEXT_H

#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPricingContext { UmiFinancialId context_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; char code[UMI_FINANCIAL_CORE_CODE_CAPACITY]; UmiFinancialDate effective_date; bool active; } UmiPricingContext;
/* Initialize the typed financial record. */ UmiStatus umi_pricing_context_init(UmiPricingContext *item,const char *id,const char *name,const char *code,UmiFinancialDate effective_date);
/* Validate the typed financial record. */ bool umi_pricing_context_is_valid(const UmiPricingContext *item);
#ifdef __cplusplus
}
#endif

#endif
