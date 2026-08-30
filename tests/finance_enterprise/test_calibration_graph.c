/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_calibration_graph.c
 *
 * PURPOSE:
 *   Implement the test calibration graph behavior for
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

#include "umicom/finance/enterprise/calibration_graph.h"

int main(void){ UmiEnterpriseCalibrationGraph g; UmiEnterpriseCalibrationJob a,b; UmiEnterpriseCalibrationDependency d; umi_enterprise_calibration_graph_init(&g); CHECK(umi_enterprise_calibration_job_init(&a,"a","a",1U)==UMI_STATUS_OK); CHECK(umi_enterprise_calibration_job_init(&b,"b","b",1U)==UMI_STATUS_OK); CHECK(umi_enterprise_calibration_graph_add_job(&g,&a)==UMI_STATUS_OK); CHECK(umi_enterprise_calibration_graph_add_job(&g,&b)==UMI_STATUS_OK); CHECK(umi_enterprise_calibration_dependency_init(&d,"a","b")==UMI_STATUS_OK); CHECK(umi_enterprise_calibration_graph_add_dependency(&g,&d)==UMI_STATUS_OK); CHECK(umi_enterprise_calibration_dependency_init(&d,"b","a")==UMI_STATUS_OK); CHECK(umi_enterprise_calibration_graph_add_dependency(&g,&d)==UMI_STATUS_INVALID_STATE); return 0; }
