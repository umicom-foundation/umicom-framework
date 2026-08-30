/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/treasury_snapshot.h
 *
 * PURPOSE:
 *   Capture aggregate cash, liquidity, risk and collateral state at a point in time.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_TREASURY_SNAPSHOT_H
#define UMICOM_FINANCE_TREASURY_TREASURY_SNAPSHOT_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryTreasurySnapshot {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t cash_minor;
    int64_t liquidity_gap_minor;
    int64_t risk_minor;
    int64_t collateral_minor;
} UmiTreasuryTreasurySnapshot;
UmiStatus umi_treasury_treasury_snapshot_init(UmiTreasuryTreasurySnapshot *value,
    const char *id,
    int64_t cash_minor,
    int64_t liquidity_gap_minor,
    int64_t risk_minor,
    int64_t collateral_minor);
bool umi_treasury_treasury_snapshot_valid(const UmiTreasuryTreasurySnapshot *value);
int64_t umi_treasury_treasury_snapshot_net_liquidity_minor(const UmiTreasuryTreasurySnapshot *value);
#ifdef __cplusplus
}
#endif
#endif
