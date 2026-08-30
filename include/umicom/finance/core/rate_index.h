/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/rate_index.h
 *
 * PURPOSE:
 *   Define reusable rate-index metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_RATE_INDEX_H
#define UMICOM_FINANCE_CORE_RATE_INDEX_H

#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRateIndex { UmiFinancialId index_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; char code[UMI_FINANCIAL_CORE_CODE_CAPACITY]; uint32_t state; bool active; } UmiRateIndex;
/* Initialize the typed financial record. */ UmiStatus umi_rate_index_init(UmiRateIndex *item,const char *id,const char *name,const char *code,uint32_t state);
/* Validate the typed financial record. */ bool umi_rate_index_is_valid(const UmiRateIndex *item);
#ifdef __cplusplus
}
#endif

#endif
