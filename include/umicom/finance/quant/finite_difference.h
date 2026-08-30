/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/finite_difference.h
 *
 * PURPOSE:
 *   Capture symmetric valuation bumps for numerical derivatives.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_FINITE_DIFFERENCE_H
#define INCLUDE_UMICOM_FINANCE_QUANT_FINITE_DIFFERENCE_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantFiniteDifference {
    double down_value;
    double base_value;
    double up_value;
    double bump;
} UmiQuantFiniteDifference;

/* Initialise and validate the finite difference contract. */
UmiStatus umi_quant_finite_difference_init(UmiQuantFiniteDifference *record, double down_value, double base_value, double up_value, double bump);

/* Return the central first derivative from symmetric bumps. */
double umi_quant_finite_difference_first_derivative(const UmiQuantFiniteDifference *record);

#ifdef __cplusplus
}
#endif

#endif
