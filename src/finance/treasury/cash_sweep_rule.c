/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/cash_sweep_rule.c
 *
 * PURPOSE:
 *   Implement define threshold-based cash sweeping between treasury accounts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/cash_sweep_rule.h"
#include <string.h>
/*
 * Initialise treasury cash sweep rule from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_cash_sweep_rule_init(UmiTreasuryCashSweepRule *value,
    const char *id,
    int64_t trigger_minor,
    int64_t target_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->trigger_minor=trigger_minor;
    value->target_minor=target_minor;
    return umi_treasury_cash_sweep_rule_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury cash sweep rule satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_cash_sweep_rule_valid(const UmiTreasuryCashSweepRule *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->trigger_minor >= value->target_minor);
}

/*
 * Provide the treasury cash sweep rule sweep minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_cash_sweep_rule_sweep_minor(const UmiTreasuryCashSweepRule *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->trigger_minor - value->target_minor;
}
