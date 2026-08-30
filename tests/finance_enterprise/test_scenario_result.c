/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_scenario_result.c
 *
 * PURPOSE:
 *   Implement the test scenario result behavior for
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

#include "umicom/finance/enterprise/scenario_result.h"

int main(void){UmiEnterpriseScenarioResult r;CHECK(umi_enterprise_scenario_result_init(&r,"s","p",-12.0)==UMI_STATUS_OK);CHECK(r.loss==12.0);return 0;}
