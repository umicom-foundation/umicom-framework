/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/monte_carlo_config.c
 *
 * PURPOSE:
 *   Configure bounded Monte Carlo simulation work.
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

#include "umicom/finance/quant/monte_carlo_config.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_monte_carlo_config_init(UmiQuantMonteCarloConfig *record, uint32_t path_count, uint32_t time_steps, uint64_t seed)
{
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(path_count > 0U && path_count <= 10000000U && time_steps > 0U && time_steps <= 4096U && seed != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->path_count = path_count;
    record->time_steps = time_steps;
    record->seed = seed;
    return UMI_STATUS_OK;
}

/* Return path-count times time-steps as execution work units. */
double umi_quant_monte_carlo_config_work_units(const UmiQuantMonteCarloConfig *record)
{
    if (record == NULL) return 0.0;
    return (double)record->path_count * (double)record->time_steps;
}
