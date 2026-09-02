/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/forward_contract.c
 *
 * PURPOSE:
 *   Represent linear forward payoff economics.
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

#include "umicom/finance/quant/forward_contract.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_forward_contract_init(UmiQuantForwardContract *record, double quantity, double strike, double spot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!(quantity >= 0.0 && strike >= 0.0 && spot >= 0.0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->quantity = quantity;
    record->strike = strike;
    record->spot = spot;
    return UMI_STATUS_OK;
}

/* Return long-forward intrinsic payoff before discounting. */
double umi_quant_forward_contract_payoff(const UmiQuantForwardContract *record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return 0.0;
    return record->quantity * (record->spot - record->strike);
}
