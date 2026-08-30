/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/treasury_snapshot.c
 *
 * PURPOSE:
 *   Implement capture aggregate cash, liquidity, risk and collateral state at a point in time.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/treasury_snapshot.h"
#include <string.h>
UmiStatus umi_treasury_treasury_snapshot_init(UmiTreasuryTreasurySnapshot *value,
    const char *id,
    int64_t cash_minor,
    int64_t liquidity_gap_minor,
    int64_t risk_minor,
    int64_t collateral_minor) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->cash_minor=cash_minor;
    value->liquidity_gap_minor=liquidity_gap_minor;
    value->risk_minor=risk_minor;
    value->collateral_minor=collateral_minor;
    return umi_treasury_treasury_snapshot_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_treasury_snapshot_valid(const UmiTreasuryTreasurySnapshot *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->collateral_minor >= 0);
}

int64_t umi_treasury_treasury_snapshot_net_liquidity_minor(const UmiTreasuryTreasurySnapshot *value) {
    if (value == NULL) return (int64_t)0;
    return value->cash_minor + value->liquidity_gap_minor;
}
