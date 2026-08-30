/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_stress_suite.c
 *
 * PURPOSE:
 *   Verify aggregate scenario identifiers into one governed enterprise stress suite.
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

#include "umicom/finance/prudential/stress_suite.h"

int main(void) { UmiPrudentialStressSuite s={0}; if(umi_pru_stress_suite_add(&s,"BASE")!=UMI_STATUS_OK)return 1; if(umi_pru_stress_suite_add(&s,"BASE")!=UMI_STATUS_ALREADY_EXISTS)return 2; return 0; }
