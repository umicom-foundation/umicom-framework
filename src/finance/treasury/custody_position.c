/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/custody_position.c
 *
 * PURPOSE:
 *   Implement represent settled, pending-in and pending-out custody quantities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/custody_position.h"
#include <string.h>
UmiStatus umi_treasury_custody_position_init(UmiTreasuryCustodyPosition *value,
    const char *id,
    int64_t settled_quantity,
    int64_t pending_in_quantity,
    int64_t pending_out_quantity) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->settled_quantity=settled_quantity;
    value->pending_in_quantity=pending_in_quantity;
    value->pending_out_quantity=pending_out_quantity;
    return umi_treasury_custody_position_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_custody_position_valid(const UmiTreasuryCustodyPosition *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->settled_quantity >= 0 && value->pending_in_quantity >= 0 && value->pending_out_quantity >= 0);
}

int64_t umi_treasury_custody_position_projected_quantity(const UmiTreasuryCustodyPosition *value) {
    if (value == NULL) return (int64_t)0;
    return value->settled_quantity + value->pending_in_quantity - value->pending_out_quantity;
}
