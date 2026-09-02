/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/stress_factor.h
 *
 * PURPOSE:
 *   Define a named multiplicative stress factor with direction and severity.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_STRESS_FACTOR_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_STRESS_FACTOR_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the prudential stress factor data shared with callers of this public contract.
 */
typedef struct UmiPrudentialStressFactor { char factor_id[UMI_PRU_ID_CAPACITY]; UmiStressDirection direction; double magnitude; UmiPrudentialSeverity severity; } UmiPrudentialStressFactor;
/* Initialise a bounded percentage stress factor. */
UmiStatus umi_pru_stress_factor_init(UmiPrudentialStressFactor *factor, const char *factor_id, UmiStressDirection direction, double magnitude, UmiPrudentialSeverity severity);

#ifdef __cplusplus
}
#endif

#endif
