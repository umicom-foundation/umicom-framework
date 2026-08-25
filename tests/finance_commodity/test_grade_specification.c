#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/commodity/grade_specification.h"

int main(void)
{
    UmiCommodityGradeSpecification value;
    CHECK(umi_commodity_grade_specification_init(&value, "GRADE-BRENT", "CMD-BRENT", "BFOET") == UMI_STATUS_OK);
    CHECK(umi_commodity_grade_specification_valid(&value));
    CHECK(value.active);
    return 0;
}
