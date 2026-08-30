/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/stress_suite.h
 *
 * PURPOSE:
 *   Aggregate scenario identifiers into one governed enterprise stress suite.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_STRESS_SUITE_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_STRESS_SUITE_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialStressSuite { char suite_id[UMI_PRU_ID_CAPACITY]; char scenario_ids[UMI_PRU_MAX_SCENARIOS][UMI_PRU_ID_CAPACITY]; size_t count; } UmiPrudentialStressSuite;
/* Append a scenario identifier to a bounded governed stress suite. */
UmiStatus umi_pru_stress_suite_add(UmiPrudentialStressSuite *suite, const char *scenario_id);

#ifdef __cplusplus
}
#endif

#endif
