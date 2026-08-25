/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/market_risk_snapshot.h
 *
 * PURPOSE:
 *   Capture aggregate market-risk value and stress loss.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_MARKET_RISK_SNAPSHOT_H
#define UMICOM_FINANCE_TREASURY_MARKET_RISK_SNAPSHOT_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryMarketRiskSnapshot {
    char id[UMI_TREASURY_ID_CAPACITY];
    UmiTreasuryRiskClass risk_class;
    int64_t primary_minor;
    int64_t secondary_minor;
} UmiTreasuryMarketRiskSnapshot;
UmiStatus umi_treasury_market_risk_snapshot_init(UmiTreasuryMarketRiskSnapshot *value,
    const char *id,
    int64_t primary_minor,
    int64_t secondary_minor);
bool umi_treasury_market_risk_snapshot_valid(const UmiTreasuryMarketRiskSnapshot *value);
int64_t umi_treasury_market_risk_snapshot_combined_absolute_minor(const UmiTreasuryMarketRiskSnapshot *value);
#ifdef __cplusplus
}
#endif
#endif
