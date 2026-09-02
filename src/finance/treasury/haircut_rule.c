/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/haircut_rule.c
 *
 * PURPOSE:
 *   Implement define collateral valuation haircut in basis points.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/haircut_rule.h"
#include <string.h>
/*
 * Initialise treasury haircut rule from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_treasury_haircut_rule_init(UmiTreasuryHaircutRule *value,
    const char *id,
    uint32_t haircut_bps) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->haircut_bps=haircut_bps;
    return umi_treasury_haircut_rule_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury haircut rule satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_haircut_rule_valid(const UmiTreasuryHaircutRule *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->haircut_bps <= 10000U);
}

/*
 * Provide the treasury haircut rule remaining bps operation used by this module and its
 * client applications.
 */
uint32_t umi_treasury_haircut_rule_remaining_bps(const UmiTreasuryHaircutRule *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (uint32_t)0;
    return 10000U - value->haircut_bps;
}
