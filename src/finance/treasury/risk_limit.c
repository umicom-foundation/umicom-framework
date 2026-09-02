/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/risk_limit.c
 *
 * PURPOSE:
 *   Implement define a hard treasury risk limit and warning threshold.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/risk_limit.h"
#include <string.h>
/*
 * Initialise treasury risk limit from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_treasury_risk_limit_init(UmiTreasuryRiskLimit *value,
    const char *id,
    int64_t hard_limit_minor,
    int64_t warning_limit_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->hard_limit_minor=hard_limit_minor;
    value->warning_limit_minor=warning_limit_minor;
    return umi_treasury_risk_limit_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury risk limit satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_risk_limit_valid(const UmiTreasuryRiskLimit *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->hard_limit_minor > 0 && value->warning_limit_minor >= 0 && value->warning_limit_minor <= value->hard_limit_minor);
}

/*
 * Provide the treasury risk limit buffer minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_risk_limit_buffer_minor(const UmiTreasuryRiskLimit *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->hard_limit_minor - value->warning_limit_minor;
}
