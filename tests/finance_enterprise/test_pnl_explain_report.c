/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_pnl_explain_report.c
 *
 * PURPOSE:
 *   Implement the test pnl explain report behavior for
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

#include "umicom/finance/enterprise/pnl_explain_report.h"

int main(void){UmiEnterprisePnlExplainReport r;UmiEnterprisePnlExplainBucket b;CHECK(umi_enterprise_pnl_explain_report_init(&r,"p",10.0)==UMI_STATUS_OK);CHECK(umi_enterprise_pnl_explain_bucket_init(&b,"delta",7.0)==UMI_STATUS_OK);CHECK(umi_enterprise_pnl_explain_report_add(&r,&b)==UMI_STATUS_OK);CHECK(umi_enterprise_pnl_explain_report_residual(&r)==3.0);return 0;}
