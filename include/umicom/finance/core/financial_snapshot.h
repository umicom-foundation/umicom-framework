/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/financial_snapshot.h
 *
 * PURPOSE:
 *   Define lightweight financial inventory snapshots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_FINANCIAL_SNAPSHOT_H
#define UMICOM_FINANCE_CORE_FINANCIAL_SNAPSHOT_H

#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiFinancialSnapshot { UmiFinancialId snapshot_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; char code[UMI_FINANCIAL_CORE_CODE_CAPACITY]; uint32_t state; bool active; } UmiFinancialSnapshot;
/* Initialize the typed financial record. */ UmiStatus umi_financial_snapshot_init(UmiFinancialSnapshot *item,const char *id,const char *name,const char *code,uint32_t state);
/* Validate the typed financial record. */ bool umi_financial_snapshot_is_valid(const UmiFinancialSnapshot *item);
#ifdef __cplusplus
}
#endif

#endif
