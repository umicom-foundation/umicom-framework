/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/portfolio_snapshot.h
 *
 * PURPOSE:
 *   Capture a coherent bounded set of portfolio positions at one time.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_PORTFOLIO_SNAPSHOT_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_PORTFOLIO_SNAPSHOT_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/finance/enterprise/portfolio_position.h"
typedef struct UmiEnterprisePortfolioSnapshot { char portfolio_id[UMI_ENTERPRISE_ID_CAPACITY]; int64_t as_of_ms; UmiEnterprisePortfolioPosition positions[UMI_ENTERPRISE_MAX_POSITIONS]; size_t count; } UmiEnterprisePortfolioSnapshot;
/* Initialise an empty portfolio snapshot. */
UmiStatus umi_enterprise_portfolio_snapshot_init(UmiEnterprisePortfolioSnapshot *snapshot,const char *portfolio_id,int64_t as_of_ms);
/* Add a unique product position to the snapshot. */
UmiStatus umi_enterprise_portfolio_snapshot_add(UmiEnterprisePortfolioSnapshot *snapshot,const UmiEnterprisePortfolioPosition *position);

#ifdef __cplusplus
}
#endif

#endif
