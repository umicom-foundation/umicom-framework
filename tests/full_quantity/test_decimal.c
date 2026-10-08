/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/full_quantity/test_decimal.c
 * PURPOSE: Cover fixed-point comparisons at opposite scales and signed arithmetic boundaries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include <stdint.h>
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    if (!strcmp(argv[1], "compare"))
    {
        const struct
        {
            int64_t a, b;
            uint8_t sa, sb;
            int expected;
        } cases[] = {{425, 4250, 2, 3, 0},
                     {425, 4260, 2, 3, -1},
                     {426, 4250, 2, 3, 1},
                     {-425, -4251, 2, 3, 1},
                     {-4251, -425, 3, 2, -1},
                     {0, 0, 9, 0, 0},
                     {INT64_MAX, INT64_MAX, 0, 9, 1},
                     {INT64_MIN, INT64_MIN, 9, 0, 1},
                     {INT64_MIN, INT64_MAX, 0, 0, -1},
                     {INT64_MIN, INT64_MIN, 0, 0, 0},
                     {-1, 0, 9, 0, -1},
                     {0, -1, 9, 0, 1},
                     {1, 1, 9, 0, -1},
                     {1, 1, 0, 9, 1}};
        for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        {
            int result = 7;
            CHECK(UmiDecimalCompare(Amount(cases[i].a, cases[i].sa), Amount(cases[i].b, cases[i].sb),
                                    &result) == UMI_STATUS_OK);
            CHECK(result == cases[i].expected);
            CHECK(UmiDecimalCompare(Amount(cases[i].b, cases[i].sb), Amount(cases[i].a, cases[i].sa),
                                    &result) == UMI_STATUS_OK);
            CHECK(result == -cases[i].expected);
        }
    }
    else if (!strcmp(argv[1], "sum"))
    {
        UmiDecimal out;
        CHECK(UmiDecimalAddExact(Amount(70, 0), Amount(125, 2), &out) == UMI_STATUS_OK);
        CHECK(out.coefficient == 7125 && out.scale == 2U);
        CHECK(UmiDecimalAddExact(Amount(INT64_MIN, 0), Amount(INT64_MAX, 0), &out) == UMI_STATUS_OK);
        CHECK(out.coefficient == -1);
        CHECK(UmiDecimalAddExact(Amount(-100, 2), Amount(1, 0), &out) == UMI_STATUS_OK);
        CHECK(out.coefficient == 0 && out.scale == 2U);
    }
    else if (!strcmp(argv[1], "overflow"))
    {
        UmiDecimal out = Amount(17, 3);
        CHECK(UmiDecimalAddExact(Amount(INT64_MAX, 0), Amount(1, 0), &out) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(out.coefficient == 17 && out.scale == 3U);
        CHECK(UmiDecimalAddExact(Amount(INT64_MIN, 0), Amount(-1, 0), &out) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiDecimalAddExact(Amount(INT64_MAX, 0), Amount(-1, 9), &out) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(out.coefficient == 17 && out.scale == 3U);
    }
    else if (!strcmp(argv[1], "invalid"))
    {
        int result = 7;
        UmiDecimal out = Amount(17, 3);
        CHECK(UmiDecimalCompare(Amount(1, 10), Amount(1, 0), &result) == UMI_STATUS_INVALID_ARGUMENT &&
              result == 7);
        CHECK(UmiDecimalCompare(Amount(1, 0), Amount(1, 0), NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDecimalAddExact(Amount(1, 0), Amount(1, 10), &out) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(out.coefficient == 17 && out.scale == 3U);
    }
    else
        return 2;
    return 0;
}
