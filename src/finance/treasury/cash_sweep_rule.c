/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/cash_sweep_rule.c
 *
 * PURPOSE:
 *   Implement define threshold-based cash sweeping between treasury accounts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/cash_sweep_rule.h"
#include <string.h>
UmiStatus umi_treasury_cash_sweep_rule_init(UmiTreasuryCashSweepRule *value,
    const char *id,
    int64_t trigger_minor,
    int64_t target_minor) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->trigger_minor=trigger_minor;
    value->target_minor=target_minor;
    return umi_treasury_cash_sweep_rule_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_cash_sweep_rule_valid(const UmiTreasuryCashSweepRule *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->trigger_minor >= value->target_minor);
}

int64_t umi_treasury_cash_sweep_rule_sweep_minor(const UmiTreasuryCashSweepRule *value) {
    if (value == NULL) return (int64_t)0;
    return value->trigger_minor - value->target_minor;
}
