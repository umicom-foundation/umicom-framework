/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_valuation_audit.c
 *
 * PURPOSE:
 *   Implement the test valuation audit behavior for
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

#include "umicom/finance/enterprise/valuation_audit.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiEnterpriseValuationAudit a;UmiEnterpriseValuationAuditEvent e={1U,{0},{0},1,UMI_STATUS_OK};umi_enterprise_valuation_audit_init(&a);CHECK(umi_quant_copy_text(e.job_id,sizeof e.job_id,"j")==UMI_STATUS_OK);CHECK(umi_quant_copy_text(e.action,sizeof e.action,"start")==UMI_STATUS_OK);CHECK(umi_enterprise_valuation_audit_append(&a,&e)==UMI_STATUS_OK);CHECK(a.count==1U);return 0;}
