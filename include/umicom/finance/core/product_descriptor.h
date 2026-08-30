/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/product_descriptor.h
 *
 * PURPOSE:
 *   Describe reusable financial products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_PRODUCT_DESCRIPTOR_H
#define UMICOM_FINANCE_CORE_PRODUCT_DESCRIPTOR_H

#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProductDescriptor { UmiFinancialId product_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; char code[UMI_FINANCIAL_CORE_CODE_CAPACITY]; uint32_t state; bool active; } UmiProductDescriptor;
/* Initialize the typed financial record. */ UmiStatus umi_product_descriptor_init(UmiProductDescriptor *item,const char *id,const char *name,const char *code,uint32_t state);
/* Validate the typed financial record. */ bool umi_product_descriptor_is_valid(const UmiProductDescriptor *item);
#ifdef __cplusplus
}
#endif

#endif
