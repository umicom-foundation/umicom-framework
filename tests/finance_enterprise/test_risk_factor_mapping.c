/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_risk_factor_mapping.c
 *
 * PURPOSE:
 *   Implement the test risk factor mapping behavior for
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

#include "umicom/finance/enterprise/risk_factor_mapping.h"

int main(void){ UmiEnterpriseRiskFactorMapping m; umi_enterprise_risk_factor_mapping_init(&m); CHECK(umi_enterprise_risk_factor_mapping_add(&m,"swap","USD01",2.0)==UMI_STATUS_OK); CHECK(umi_enterprise_risk_factor_mapping_loading(&m,"swap","USD01")==2.0); return 0; }
