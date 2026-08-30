/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/recovery_rate.h
 *
 * PURPOSE:
 *   Represent default recovery and loss-given-default.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_RECOVERY_RATE_H
#define INCLUDE_UMICOM_FINANCE_QUANT_RECOVERY_RATE_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantRecoveryRate {
    double recovery;
} UmiQuantRecoveryRate;

/* Initialise and validate the recovery rate contract. */
UmiStatus umi_quant_recovery_rate_init(UmiQuantRecoveryRate *record, double recovery);

/* Return one minus recovery rate. */
double umi_quant_recovery_rate_loss_given_default(const UmiQuantRecoveryRate *record);

#ifdef __cplusplus
}
#endif

#endif
