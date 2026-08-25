/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/margin_requirement.c
 *
 * PURPOSE:
 *   Implement calculate required margin after threshold and independent amount.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/margin_requirement.h"
#include <string.h>
UmiStatus umi_treasury_margin_requirement_init(UmiTreasuryMarginRequirement *value,
    const char *id,
    int64_t exposure_minor,
    int64_t threshold_minor,
    int64_t independent_amount_minor) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->exposure_minor=exposure_minor;
    value->threshold_minor=threshold_minor;
    value->independent_amount_minor=independent_amount_minor;
    return umi_treasury_margin_requirement_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_margin_requirement_valid(const UmiTreasuryMarginRequirement *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->exposure_minor >= 0 && value->threshold_minor >= 0 && value->independent_amount_minor >= 0);
}

int64_t umi_treasury_margin_requirement_required_minor(const UmiTreasuryMarginRequirement *value) {
    if (value == NULL) return (int64_t)0;
    return (value->exposure_minor > value->threshold_minor ? value->exposure_minor - value->threshold_minor : 0) + value->independent_amount_minor;
}
