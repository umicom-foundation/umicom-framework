/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/loss_allowance.c
 *
 * PURPOSE:
 *   Implement permitted physical loss as basis points of shipped quantity.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/commodity/loss_allowance.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_loss_allowance_init(UmiCommodityLossAllowance *value, const char *contract_id, int32_t basis_points)
{
    UmiStatus status;
    if (value == NULL || basis_points < 0 || basis_points > 10000) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->contract_id.value, sizeof value->contract_id.value, contract_id);
    if (status != UMI_STATUS_OK) return status;
    value->basis_points = basis_points;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_loss_allowance_valid(const UmiCommodityLossAllowance *value)
{
    return value != NULL && (umi_commodity_text_valid(value->contract_id.value) && value->basis_points >= 0 && value->basis_points <= 10000 && value->active);
}
