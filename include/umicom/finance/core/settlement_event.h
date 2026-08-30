/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/settlement_event.h
 *
 * PURPOSE:
 *   Describe settlement state evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_SETTLEMENT_EVENT_H
#define UMICOM_FINANCE_CORE_SETTLEMENT_EVENT_H

#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSettlementEvent { UmiFinancialId event_id; UmiFinancialId parent_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; UmiFinancialDate effective_date; uint32_t state; bool active; } UmiSettlementEvent;
/* Initialize the typed financial record. */ UmiStatus umi_settlement_event_init(UmiSettlementEvent *item,const char *id,const char *name,const char *parent_id,UmiFinancialDate effective_date,uint32_t state);
/* Validate the typed financial record. */ bool umi_settlement_event_is_valid(const UmiSettlementEvent *item);
#ifdef __cplusplus
}
#endif

#endif
