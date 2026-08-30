/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/funding_source.c
 *
 * PURPOSE:
 *   Implement model a funding facility with capacity, drawn amount and cost.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/funding_source.h"
#include <string.h>
UmiStatus umi_treasury_funding_source_init(UmiTreasuryFundingSource *value,
    const char *id,
    int64_t capacity_minor,
    int64_t drawn_minor,
    int32_t spread_bps) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->capacity_minor=capacity_minor;
    value->drawn_minor=drawn_minor;
    value->spread_bps=spread_bps;
    return umi_treasury_funding_source_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_funding_source_valid(const UmiTreasuryFundingSource *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->capacity_minor >= 0 && value->drawn_minor >= 0 && value->drawn_minor <= value->capacity_minor && value->spread_bps >= 0);
}

int64_t umi_treasury_funding_source_available_minor(const UmiTreasuryFundingSource *value) {
    if (value == NULL) return (int64_t)0;
    return value->capacity_minor - value->drawn_minor;
}
