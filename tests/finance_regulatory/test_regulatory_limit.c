/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_regulatory_limit.c
 *
 * PURPOSE:
 *   Implement the test regulatory limit behavior for
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

#include "umicom/finance/regulatory/regulatory_limit.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiRegulatoryLimit record;
    CHECK(umi_reg_regulatory_limit_init(&record, "limit_id-1", "Metric", 0.95, 1) == UMI_STATUS_OK);
    CHECK(record.limit_id[0] != '\0');
    CHECK(record.metric[0] != '\0');
    return 0;
}
