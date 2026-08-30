/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/currency_descriptor.h
 *
 * PURPOSE:
 *   Describe ISO-style currency metadata while reusing the existing UmiCurrency type.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_CURRENCY_DESCRIPTOR_H
#define UMICOM_FINANCE_CORE_CURRENCY_DESCRIPTOR_H

#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiCurrencyDescriptor { UmiCurrency currency; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; uint8_t minor_scale; bool active; } UmiCurrencyDescriptor;
/* Initialize normalized currency metadata. */ UmiStatus umi_currency_descriptor_init(UmiCurrencyDescriptor *d,const char *code,const char *name,uint8_t scale);
/* Validate normalized currency metadata. */ bool umi_currency_descriptor_is_valid(const UmiCurrencyDescriptor *d);
/* Match a three-letter currency code case-insensitively. */ bool umi_currency_descriptor_matches(const UmiCurrencyDescriptor *d,const char *code);
#ifdef __cplusplus
}
#endif

#endif
