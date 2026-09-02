/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/portfolio.h
 *
 * PURPOSE:
 *   Define portfolio groupings under books.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_PORTFOLIO_H
#define UMICOM_FINANCE_CORE_PORTFOLIO_H

#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the financial portfolio data shared with callers of this public contract.
 */
typedef struct UmiFinancialPortfolio { UmiFinancialId portfolio_id; UmiFinancialId parent_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; bool active; } UmiFinancialPortfolio;
/* Initialize the typed financial record. */ UmiStatus umi_portfolio_init(UmiFinancialPortfolio *item,const char *id,const char *name,const char *parent_id);
/* Validate the typed financial record. */ bool umi_portfolio_is_valid(const UmiFinancialPortfolio *item);
#ifdef __cplusplus
}
#endif

#endif
