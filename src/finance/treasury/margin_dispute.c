/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/margin_dispute.c
 *
 * PURPOSE:
 *   Implement track margin dispute amount and resolution state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/margin_dispute.h"
#include <string.h>
UmiStatus umi_treasury_margin_dispute_init(UmiTreasuryMarginDispute *value,
    const char *id,
    int64_t disputed_minor,
    int64_t resolved_minor) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->disputed_minor=disputed_minor;
    value->resolved_minor=resolved_minor;
    return umi_treasury_margin_dispute_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_margin_dispute_valid(const UmiTreasuryMarginDispute *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->disputed_minor >= 0 && value->resolved_minor >= 0 && value->resolved_minor <= value->disputed_minor);
}

int64_t umi_treasury_margin_dispute_outstanding_minor(const UmiTreasuryMarginDispute *value) {
    if (value == NULL) return (int64_t)0;
    return value->disputed_minor - value->resolved_minor;
}
