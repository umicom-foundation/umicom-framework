/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/settlement_repair.c
 *
 * PURPOSE:
 *   Implement represent a settlement repair action with bounded attempt governance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/settlement_repair.h"
#include <string.h>
/*
 * Initialise treasury settlement repair from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_settlement_repair_init(UmiTreasurySettlementRepair *value,
    const char *id,
    uint32_t attempt,
    uint32_t maximum_attempts) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->attempt=attempt;
    value->maximum_attempts=maximum_attempts;
    return umi_treasury_settlement_repair_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury settlement repair satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_settlement_repair_valid(const UmiTreasurySettlementRepair *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->maximum_attempts > 0U && value->attempt <= value->maximum_attempts);
}

/*
 * Provide the treasury settlement repair retry allowed operation used by this module and
 * its client applications.
 */
bool umi_treasury_settlement_repair_retry_allowed(const UmiTreasurySettlementRepair *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (bool)0;
    return value->attempt < value->maximum_attempts;
}
