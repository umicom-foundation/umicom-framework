/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_stress_suite.c
 *
 * PURPOSE:
 *   Implement the test stress suite behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/stress_suite.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiEnterpriseStressSuite suite;UmiEnterpriseStressScenario s;CHECK(umi_enterprise_stress_suite_init(&suite,"reg")==UMI_STATUS_OK);CHECK(umi_enterprise_stress_scenario_init(&s,"x","USD",0.01,3)==UMI_STATUS_OK);CHECK(umi_enterprise_stress_suite_add(&suite,&s)==UMI_STATUS_OK);CHECK(umi_enterprise_stress_suite_max_severity(&suite)==3);return 0;}
