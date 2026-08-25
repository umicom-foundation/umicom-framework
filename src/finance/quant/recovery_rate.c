/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/recovery_rate.c
 *
 * PURPOSE:
 *   Represent default recovery and loss-given-default.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/recovery_rate.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_recovery_rate_init(UmiQuantRecoveryRate *record, double recovery)
{
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(recovery >= 0.0 && recovery <= 1.0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->recovery = recovery;
    return UMI_STATUS_OK;
}

/* Return one minus recovery rate. */
double umi_quant_recovery_rate_loss_given_default(const UmiQuantRecoveryRate *record)
{
    if (record == NULL) return 0.0;
    return 1.0 - record->recovery;
}
