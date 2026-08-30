/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_reporting_audit.c
 *
 * PURPOSE:
 *   Implement the test reporting audit behavior for
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
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/reporting_audit.h"

int main(void){ UmiReportingAudit a; umi_reg_reporting_audit_init(&a); CHECK(umi_reg_reporting_audit_append(&a,1U,"validate",123U)==UMI_STATUS_OK); CHECK(umi_reg_reporting_audit_append(&a,1U,"submit",124U)==UMI_STATUS_INVALID_STATE); return 0; }
