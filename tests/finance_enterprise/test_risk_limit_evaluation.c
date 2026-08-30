/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_risk_limit_evaluation.c
 *
 * PURPOSE:
 *   Implement the test risk limit evaluation behavior for
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

#include "umicom/finance/enterprise/risk_limit_evaluation.h"

int main(void){UmiEnterpriseRiskLimitEvaluation e;CHECK(umi_enterprise_risk_limit_evaluate(&e,"VAR",80.0,100.0,90.0)==UMI_STATUS_OK);CHECK(e.warning==1&&e.breach==0);return 0;}
