/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_capital_measure.c
 *
 * PURPOSE:
 *   Implement the test capital measure behavior for
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

#include "umicom/finance/regulatory/capital_measure.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiCapitalMeasure record;
    CHECK(umi_reg_capital_measure_init(&record, "measure_id-1", "Name", 1.25, (UmiCurrency){"USD"}) == UMI_STATUS_OK);
    CHECK(record.measure_id[0] != '\0');
    CHECK(record.name[0] != '\0');
    return 0;
}
