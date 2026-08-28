/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/financial_core_service.h
 *
 * PURPOSE:
 *   Aggregate canonical shared financial reference-data registries into one low-level Framework service.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_FINANCIAL_CORE_SERVICE_H
#define UMICOM_FINANCE_CORE_FINANCIAL_CORE_SERVICE_H

#include "umicom/finance/core/currency_catalogue.h"
#include "umicom/finance/core/legal_entity_registry.h"
#include "umicom/finance/core/counterparty_registry.h"
#include "umicom/finance/core/book_registry.h"
#include "umicom/finance/core/portfolio_registry.h"
#include "umicom/finance/core/account_registry.h"
#include "umicom/finance/core/product_registry.h"
#include "umicom/finance/core/trade_registry.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiFinancialCoreService { UmiCurrencyCatalogue currencies; UmiLegalEntityRegistry legal_entities; UmiCounterpartyRegistry counterparties; UmiBookRegistry books; UmiPortfolioRegistry portfolios; UmiAccountRegistry accounts; UmiProductRegistry products; UmiTradeRegistry trades; uint64_t revision; bool ready; } UmiFinancialCoreService;
/* Initialize service. */ void umi_financial_core_service_init(UmiFinancialCoreService *s);
/* Start service after reference data configuration. */ UmiStatus umi_financial_core_service_start(UmiFinancialCoreService *s);
/* Return total registered reference-data records. */ size_t umi_financial_core_service_inventory(const UmiFinancialCoreService *s);
#ifdef __cplusplus
}
#endif

#endif
