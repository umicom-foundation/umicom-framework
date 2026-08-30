/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/limit_breach.c
 *
 * PURPOSE:
 *   Implement record risk-limit breaches and acknowledgement state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/limit_breach.h"
#include <string.h>
UmiStatus umi_treasury_limit_breach_init(UmiTreasuryLimitBreach *value,
    const char *id,
    int64_t excess_minor,
    bool acknowledged) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->excess_minor=excess_minor;
    value->acknowledged=acknowledged;
    return umi_treasury_limit_breach_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_limit_breach_valid(const UmiTreasuryLimitBreach *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->excess_minor > 0);
}

bool umi_treasury_limit_breach_open(const UmiTreasuryLimitBreach *value) {
    if (value == NULL) return (bool)0;
    return !value->acknowledged;
}
