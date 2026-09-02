/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/treasury_service.h
 *
 * PURPOSE:
 *   Aggregate treasury, risk, settlement, collateral and custody readiness in a Framework-owned service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_TREASURY_SERVICE_H
#define UMICOM_FINANCE_TREASURY_TREASURY_SERVICE_H
#include "umicom/finance/treasury/treasury_snapshot.h"
#include "umicom/finance/treasury/risk_limit_book.h"
#include "umicom/finance/treasury/margin_call_book.h"
#include "umicom/finance/treasury/settlement_queue.h"
/**
 * Represent the treasury treasury service data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryTreasuryService { bool running; bool maintenance; uint32_t blockers; UmiTreasuryTreasurySnapshot snapshot; } UmiTreasuryTreasuryService;
/**
 * Initialise treasury treasury service from caller-provided values so later operations
 * receive a known state.
 */
void umi_treasury_treasury_service_init(UmiTreasuryTreasuryService *service);
/**
 * Provide the treasury treasury service start operation used by this module and its client
 * applications.
 */
UmiStatus umi_treasury_treasury_service_start(UmiTreasuryTreasuryService *service,const UmiTreasuryTreasurySnapshot *snapshot);
/**
 * Provide the treasury treasury service set blockers operation used by this module and its
 * client applications.
 */
void umi_treasury_treasury_service_set_blockers(UmiTreasuryTreasuryService *service,uint32_t blockers);
/**
 * Provide the treasury treasury service ready operation used by this module and its client
 * applications.
 */
bool umi_treasury_treasury_service_ready(const UmiTreasuryTreasuryService *service);
#endif
