/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/bond_schedule.c
 *
 * PURPOSE:
 *   Describe regular fixed-income payment intervals.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/bond_schedule.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_bond_schedule_init(UmiQuantBondSchedule *record, int32_t maturity_days, int32_t frequency_days)
{
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(maturity_days > 0 && frequency_days > 0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->maturity_days = maturity_days;
    record->frequency_days = frequency_days;
    return UMI_STATUS_OK;
}

/* Return the ceiling number of scheduled payment periods. */
double umi_quant_bond_schedule_period_count(const UmiQuantBondSchedule *record)
{
    if (record == NULL) return 0.0;
    return (double)((record->maturity_days + record->frequency_days - 1) / record->frequency_days);
}
