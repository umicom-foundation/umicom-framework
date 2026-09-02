/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/liquidity_measure.c
 *
 * PURPOSE:
 *   Represent a regulatory liquidity stock, outflow or inflow measure.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/liquidity_measure.h"

#include <string.h>

/*
 * Initialise reg liquidity measure from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_reg_liquidity_measure_init(UmiLiquidityMeasure *record, const char *measure_id, const char *name, double amount, UmiCurrency currency)
{
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || !(umi_reg_number_valid(amount))) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->measure_id, sizeof record->measure_id, measure_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->name, sizeof record->name, name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    record->amount = amount;
    record->currency = currency;
    return UMI_STATUS_OK;
}
