/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/portfolio_position.h
 *
 * PURPOSE:
 *   Represent a quantity and valuation contribution for one portfolio product.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_PORTFOLIO_POSITION_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_PORTFOLIO_POSITION_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEnterprisePortfolioPosition { UmiFinancialId product_id; double quantity; double unit_value; double market_value; } UmiEnterprisePortfolioPosition;
/* Initialise a finite portfolio position and derive its market value. */
UmiStatus umi_enterprise_portfolio_position_init(UmiEnterprisePortfolioPosition *position,const char *product_id,double quantity,double unit_value);

#ifdef __cplusplus
}
#endif

#endif
