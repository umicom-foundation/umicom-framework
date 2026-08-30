/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/portfolio_registry.h
 *
 * PURPOSE:
 *   Provide portfolio lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_PORTFOLIO_REGISTRY_H
#define UMICOM_FINANCE_CORE_PORTFOLIO_REGISTRY_H

#include "umicom/finance/core/portfolio.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPortfolioRegistry { UmiFinancialPortfolio items[UMI_FINANCIAL_CORE_MAX_ITEMS]; size_t count; } UmiPortfolioRegistry;
/* Reset the bounded registry. */ void umi_portfolio_registry_init(UmiPortfolioRegistry *registry);
/* Add a unique item. */ UmiStatus umi_portfolio_registry_add(UmiPortfolioRegistry *registry,const UmiFinancialPortfolio *item);
/* Find an item by identifier. */ const UmiFinancialPortfolio *umi_portfolio_registry_find(const UmiPortfolioRegistry *registry,const char *id);
#ifdef __cplusplus
}
#endif

#endif
