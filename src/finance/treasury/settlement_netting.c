/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/settlement_netting.c
 *
 * PURPOSE:
 *   Implement calculate bilateral settlement netting across gross pay and receive legs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/settlement_netting.h"
#include <string.h>
/*
 * Initialise treasury settlement netting from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_settlement_netting_init(UmiTreasurySettlementNetting *value,
    const char *id,
    int64_t pay_minor,
    int64_t receive_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->pay_minor=pay_minor;
    value->receive_minor=receive_minor;
    return umi_treasury_settlement_netting_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury settlement netting satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_settlement_netting_valid(const UmiTreasurySettlementNetting *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->pay_minor >= 0 && value->receive_minor >= 0);
}

/*
 * Provide the treasury settlement netting net minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_settlement_netting_net_minor(const UmiTreasurySettlementNetting *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->receive_minor - value->pay_minor;
}
