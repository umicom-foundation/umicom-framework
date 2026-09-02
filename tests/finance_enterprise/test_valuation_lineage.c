/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_valuation_lineage.c
 *
 * PURPOSE:
 *   Implement the test valuation lineage behavior for
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

#include "umicom/finance/enterprise/valuation_lineage.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiEnterpriseValuationLineage l;CHECK(umi_enterprise_valuation_lineage_init(&l,"j",1U,"black76","abc")==UMI_STATUS_OK);CHECK(l.lineage_fingerprint!=0U);return 0;}
