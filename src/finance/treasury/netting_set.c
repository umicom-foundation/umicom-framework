/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/netting_set.c
 *
 * PURPOSE:
 *   Implement define gross receivables and payables within a legally enforceable netting set.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/netting_set.h"
#include <string.h>
/*
 * Initialise treasury netting set from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_treasury_netting_set_init(UmiTreasuryNettingSet *value,
    const char *id,
    int64_t receivable_minor,
    int64_t payable_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->receivable_minor=receivable_minor;
    value->payable_minor=payable_minor;
    return umi_treasury_netting_set_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury netting set satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_netting_set_valid(const UmiTreasuryNettingSet *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->receivable_minor >= 0 && value->payable_minor >= 0);
}

/*
 * Provide the treasury netting set net minor operation used by this module and its client
 * applications.
 */
int64_t umi_treasury_netting_set_net_minor(const UmiTreasuryNettingSet *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->receivable_minor - value->payable_minor;
}
