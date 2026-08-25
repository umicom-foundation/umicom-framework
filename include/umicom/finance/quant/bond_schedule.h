/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/bond_schedule.h
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_BOND_SCHEDULE_H
#define INCLUDE_UMICOM_FINANCE_QUANT_BOND_SCHEDULE_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantBondSchedule {
    int32_t maturity_days;
    int32_t frequency_days;
} UmiQuantBondSchedule;

/* Initialise and validate the bond schedule contract. */
UmiStatus umi_quant_bond_schedule_init(UmiQuantBondSchedule *record, int32_t maturity_days, int32_t frequency_days);

/* Return the ceiling number of scheduled payment periods. */
double umi_quant_bond_schedule_period_count(const UmiQuantBondSchedule *record);

#ifdef __cplusplus
}
#endif

#endif
