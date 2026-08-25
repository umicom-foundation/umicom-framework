/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/stress_test.h
 *
 * PURPOSE:
 *   Represent baseline and stressed portfolio valuations.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_STRESS_TEST_H
#define INCLUDE_UMICOM_FINANCE_QUANT_STRESS_TEST_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantStressTest {
    double baseline_pv;
    double stressed_pv;
    double limit_loss;
} UmiQuantStressTest;

/* Initialise and validate the stress test contract. */
UmiStatus umi_quant_stress_test_init(UmiQuantStressTest *record, double baseline_pv, double stressed_pv, double limit_loss);

/* Return positive valuation loss under stress. */
double umi_quant_stress_test_loss(const UmiQuantStressTest *record);

#ifdef __cplusplus
}
#endif

#endif
