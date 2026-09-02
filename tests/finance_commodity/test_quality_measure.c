/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_quality_measure.c
 *
 * PURPOSE:
 *   Implement the test quality measure behavior for
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

#include "umicom/finance/commodity/quality_measure.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiCommodityQualityMeasure value;
    CHECK(umi_commodity_quality_measure_init(&value, "sulphur", "PCT", 0, 50, 2) == UMI_STATUS_OK);
    CHECK(umi_commodity_quality_measure_valid(&value));
    CHECK(value.maximum == 50);
    return 0;
}
