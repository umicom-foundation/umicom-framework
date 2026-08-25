/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/liquidity_risk_snapshot.h
 *
 * PURPOSE:
 *   Capture liquidity-risk gap and liquidity buffer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_LIQUIDITY_RISK_SNAPSHOT_H
#define UMICOM_FINANCE_TREASURY_LIQUIDITY_RISK_SNAPSHOT_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryLiquidityRiskSnapshot {
    char id[UMI_TREASURY_ID_CAPACITY];
    UmiTreasuryRiskClass risk_class;
    int64_t primary_minor;
    int64_t secondary_minor;
} UmiTreasuryLiquidityRiskSnapshot;
UmiStatus umi_treasury_liquidity_risk_snapshot_init(UmiTreasuryLiquidityRiskSnapshot *value,
    const char *id,
    int64_t primary_minor,
    int64_t secondary_minor);
bool umi_treasury_liquidity_risk_snapshot_valid(const UmiTreasuryLiquidityRiskSnapshot *value);
int64_t umi_treasury_liquidity_risk_snapshot_combined_absolute_minor(const UmiTreasuryLiquidityRiskSnapshot *value);
#ifdef __cplusplus
}
#endif
#endif
