/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/option_contract.c
 *
 * PURPOSE:
 *   Represent vanilla option intrinsic economics.
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

#include "umicom/finance/quant/option_contract.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_option_contract_init(UmiQuantOptionContract *record, double quantity, double strike, double underlying, int32_t right)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!(quantity >= 0.0 && strike >= 0.0 && underlying >= 0.0 && (right == 1 || right == -1))) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->quantity = quantity;
    record->strike = strike;
    record->underlying = underlying;
    record->right = right;
    return UMI_STATUS_OK;
}

/* Return call or put intrinsic value for the configured quantity. */
double umi_quant_option_contract_intrinsic(const UmiQuantOptionContract *record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return 0.0;
    return record->quantity * fmax(((double)record->right) * (record->underlying - record->strike), 0.0);
}
