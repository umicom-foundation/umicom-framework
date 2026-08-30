/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_risk_factor_vector.c
 *
 * PURPOSE:
 *   Implement the test risk factor vector behavior for
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

#include "umicom/finance/enterprise/risk_factor_vector.h"

int main(void){ UmiEnterpriseRiskFactorVector v; double x=0.0; umi_enterprise_risk_factor_vector_init(&v); CHECK(umi_enterprise_risk_factor_vector_add(&v,"USD01",3.0)==UMI_STATUS_OK); CHECK(umi_enterprise_risk_factor_vector_get(&v,"USD01",&x)==UMI_STATUS_OK); CHECK(x==3.0); return 0; }
