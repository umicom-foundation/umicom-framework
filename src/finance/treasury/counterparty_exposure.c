/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/counterparty_exposure.c
 *
 * PURPOSE:
 *   Implement represent counterparty current and potential future exposure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/counterparty_exposure.h"
#include <string.h>
/*
 * Initialise treasury counterparty exposure from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_treasury_counterparty_exposure_init(UmiTreasuryCounterpartyExposure *value,
    const char *id,
    int64_t current_minor,
    int64_t potential_future_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->current_minor=current_minor;
    value->potential_future_minor=potential_future_minor;
    return umi_treasury_counterparty_exposure_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury counterparty exposure satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_counterparty_exposure_valid(const UmiTreasuryCounterpartyExposure *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->current_minor >= 0 && value->potential_future_minor >= 0);
}

/*
 * Provide the treasury counterparty exposure total minor operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_counterparty_exposure_total_minor(const UmiTreasuryCounterpartyExposure *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->current_minor + value->potential_future_minor;
}
