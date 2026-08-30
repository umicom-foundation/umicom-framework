/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/settlement_window.c
 *
 * PURPOSE:
 *   Implement define operational settlement opening and cut-off timestamps.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/settlement_window.h"
#include <string.h>
UmiStatus umi_treasury_settlement_window_init(UmiTreasurySettlementWindow *value,
    const char *id,
    int64_t opens_epoch_millis,
    int64_t closes_epoch_millis) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->opens_epoch_millis=opens_epoch_millis;
    value->closes_epoch_millis=closes_epoch_millis;
    return umi_treasury_settlement_window_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_settlement_window_valid(const UmiTreasurySettlementWindow *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->opens_epoch_millis >= 0 && value->closes_epoch_millis >= value->opens_epoch_millis);
}

int64_t umi_treasury_settlement_window_duration_millis(const UmiTreasurySettlementWindow *value) {
    if (value == NULL) return (int64_t)0;
    return value->closes_epoch_millis - value->opens_epoch_millis;
}
