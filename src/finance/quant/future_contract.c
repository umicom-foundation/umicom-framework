/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/future_contract.c
 *
 * PURPOSE:
 *   Represent futures mark-to-market economics.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/future_contract.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_future_contract_init(UmiQuantFutureContract *record, double quantity, double multiplier, double entry_price, double mark_price)
{
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(quantity >= 0.0 && multiplier > 0.0 && entry_price >= 0.0 && mark_price >= 0.0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->quantity = quantity;
    record->multiplier = multiplier;
    record->entry_price = entry_price;
    record->mark_price = mark_price;
    return UMI_STATUS_OK;
}

/* Return long-side futures mark-to-market value. */
double umi_quant_future_contract_mark_to_market(const UmiQuantFutureContract *record)
{
    if (record == NULL) return 0.0;
    return record->quantity * record->multiplier * (record->mark_price - record->entry_price);
}
