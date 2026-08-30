/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/collateral_allocation.c
 *
 * PURPOSE:
 *   Implement represent collateral allocation against a secured requirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/collateral_allocation.h"
#include <string.h>
UmiStatus umi_treasury_collateral_allocation_init(UmiTreasuryCollateralAllocation *value,
    const char *id,
    int64_t required_minor,
    int64_t allocated_minor) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->required_minor=required_minor;
    value->allocated_minor=allocated_minor;
    return umi_treasury_collateral_allocation_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_collateral_allocation_valid(const UmiTreasuryCollateralAllocation *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->required_minor >= 0 && value->allocated_minor >= 0);
}

int64_t umi_treasury_collateral_allocation_shortfall_minor(const UmiTreasuryCollateralAllocation *value) {
    if (value == NULL) return (int64_t)0;
    return value->required_minor > value->allocated_minor ? value->required_minor - value->allocated_minor : 0;
}
