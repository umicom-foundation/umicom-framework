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
UmiStatus umi_treasury_exposure_init(UmiTreasuryExposure *value,
    const char *id,
    int64_t gross_minor,
    int64_t net_minor) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->gross_minor=gross_minor;
    value->net_minor=net_minor;
    return umi_treasury_exposure_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_exposure_valid(const UmiTreasuryExposure *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->gross_minor >= 0 && umi_treasury_abs_i64(value->net_minor) <= value->gross_minor);
}

int64_t umi_treasury_exposure_net_absolute_minor(const UmiTreasuryExposure *value) {
    if (value == NULL) return (int64_t)0;
    return umi_treasury_abs_i64(value->net_minor);
}
