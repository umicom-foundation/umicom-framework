/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/treasury_service.h
 *
 * PURPOSE:
 *   Aggregate treasury, risk, settlement, collateral and custody readiness in a Framework-owned service.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_TREASURY_SERVICE_H
#define UMICOM_FINANCE_TREASURY_TREASURY_SERVICE_H
#include "umicom/finance/treasury/treasury_snapshot.h"
#include "umicom/finance/treasury/risk_limit_book.h"
#include "umicom/finance/treasury/margin_call_book.h"
#include "umicom/finance/treasury/settlement_queue.h"
typedef struct UmiTreasuryTreasuryService { bool running; bool maintenance; uint32_t blockers; UmiTreasuryTreasurySnapshot snapshot; } UmiTreasuryTreasuryService;
void umi_treasury_treasury_service_init(UmiTreasuryTreasuryService *service);
UmiStatus umi_treasury_treasury_service_start(UmiTreasuryTreasuryService *service,const UmiTreasuryTreasurySnapshot *snapshot);
void umi_treasury_treasury_service_set_blockers(UmiTreasuryTreasuryService *service,uint32_t blockers);
bool umi_treasury_treasury_service_ready(const UmiTreasuryTreasuryService *service);
#endif
