/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_calibration_audit.c
 *
 * PURPOSE:
 *   Implement the test calibration audit behavior for
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

#include "umicom/finance/enterprise/calibration_audit.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEnterpriseCalibrationAudit a; UmiEnterpriseCalibrationAuditEvent e={1U,{0},0.01,10}; umi_enterprise_calibration_audit_init(&a); CHECK(umi_quant_copy_text(e.job_id,sizeof e.job_id,"j")==UMI_STATUS_OK); CHECK(umi_enterprise_calibration_audit_append(&a,&e)==UMI_STATUS_OK); CHECK(a.count==1U); return 0; }
