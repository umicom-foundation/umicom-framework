/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/counterparty.h
 *
 * PURPOSE:
 *   Define counterparties separately from legal-entity identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_COUNTERPARTY_H
#define UMICOM_FINANCE_CORE_COUNTERPARTY_H

#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the counterparty data shared with callers of this public contract.
 */
typedef struct UmiCounterparty { UmiFinancialId counterparty_id; UmiFinancialId parent_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; char code[UMI_FINANCIAL_CORE_CODE_CAPACITY]; bool active; } UmiCounterparty;
/* Initialize the typed financial record. */ UmiStatus umi_counterparty_init(UmiCounterparty *item,const char *id,const char *name,const char *parent_id,const char *code);
/* Validate the typed financial record. */ bool umi_counterparty_is_valid(const UmiCounterparty *item);
#ifdef __cplusplus
}
#endif

#endif
