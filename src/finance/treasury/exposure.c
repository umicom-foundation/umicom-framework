/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/exposure.c
 *
 * PURPOSE:
 *   Implement represent gross and net financial exposure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/exposure.h"
#include <string.h>
/*
 * Initialise treasury exposure from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_treasury_exposure_init(UmiTreasuryExposure *value,
    const char *id,
    int64_t gross_minor,
    int64_t net_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->gross_minor=gross_minor;
    value->net_minor=net_minor;
    return umi_treasury_exposure_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/* Check that treasury exposure satisfies its contract before another service relies on it. */
bool umi_treasury_exposure_valid(const UmiTreasuryExposure *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->gross_minor >= 0 && umi_treasury_abs_i64(value->net_minor) <= value->gross_minor);
}

/*
 * Provide the treasury exposure net absolute minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_exposure_net_absolute_minor(const UmiTreasuryExposure *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return umi_treasury_abs_i64(value->net_minor);
}
