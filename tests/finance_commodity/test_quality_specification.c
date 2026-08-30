/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_quality_specification.c
 *
 * PURPOSE:
 *   Implement the test quality specification behavior for
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

#include "umicom/finance/commodity/quality_specification.h"

int main(void)
{
    UmiCommodityQualitySpecification specification;
    UmiCommodityQualityMeasure measure;
    CHECK(umi_commodity_quality_specification_init(&specification, "GRADE-A") == UMI_STATUS_OK);
    CHECK(umi_commodity_quality_measure_init(&measure, "density", "API", 300, 500, 1) == UMI_STATUS_OK);
    CHECK(umi_commodity_quality_specification_add(&specification, &measure) == UMI_STATUS_OK);
    CHECK(umi_commodity_quality_specification_valid(&specification));
    return 0;
}
