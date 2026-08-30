/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/credit_default_swap.c
 *
 * PURPOSE:
 *   Represent simplified CDS expected-loss economics.
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

#include "umicom/finance/quant/credit_default_swap.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_credit_default_swap_init(UmiQuantCreditDefaultSwap *record, double notional, double default_probability, double recovery_rate)
{
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(notional >= 0.0 && default_probability >= 0.0 && default_probability <= 1.0 && recovery_rate >= 0.0 && recovery_rate <= 1.0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->notional = notional;
    record->default_probability = default_probability;
    record->recovery_rate = recovery_rate;
    return UMI_STATUS_OK;
}

/* Return expected loss from default probability and recovery. */
double umi_quant_credit_default_swap_expected_loss(const UmiQuantCreditDefaultSwap *record)
{
    if (record == NULL) return 0.0;
    return record->notional * record->default_probability * (1.0 - record->recovery_rate);
}
