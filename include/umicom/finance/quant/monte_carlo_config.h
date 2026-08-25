/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/monte_carlo_config.h
 *
 * PURPOSE:
 *   Configure bounded Monte Carlo simulation work.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_MONTE_CARLO_CONFIG_H
#define INCLUDE_UMICOM_FINANCE_QUANT_MONTE_CARLO_CONFIG_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantMonteCarloConfig {
    uint32_t path_count;
    uint32_t time_steps;
    uint64_t seed;
} UmiQuantMonteCarloConfig;

/* Initialise and validate the monte carlo config contract. */
UmiStatus umi_quant_monte_carlo_config_init(UmiQuantMonteCarloConfig *record, uint32_t path_count, uint32_t time_steps, uint64_t seed);

/* Return path-count times time-steps as execution work units. */
double umi_quant_monte_carlo_config_work_units(const UmiQuantMonteCarloConfig *record);

#ifdef __cplusplus
}
#endif

#endif
