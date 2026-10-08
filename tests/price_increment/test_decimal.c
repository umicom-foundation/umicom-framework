/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/price_increment/test_decimal.c
 * PURPOSE: Cover exact exponent parsing, precision refusal and divisibility at coefficient limits.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include <stdint.h>
static int Parsed(const char *text, int64_t coefficient, uint8_t scale)
{
    UmiDecimal value = Amount(99, 9);
    CHECK(UmiDecimalParseScientificExact(text, strlen(text), &value) == UMI_STATUS_OK);
    CHECK(value.coefficient == coefficient && value.scale == scale);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    if (!strcmp(mode, "scientific"))
    {
        CHECK(Parsed("1e-4", 1, 4) == 0);
        CHECK(Parsed("+12.50E+2", 1250, 0) == 0);
        CHECK(Parsed("-12500e-3", -125, 1) == 0);
        CHECK(Parsed("0e1000", 0, 0) == 0);
        CHECK(Parsed("000.0000000010", 1, 9) == 0);
        CHECK(Parsed("-0.000", 0, 0) == 0);
    }
    else if (!strcmp(mode, "limits"))
    {
        CHECK(Parsed("9223372036854775807", INT64_MAX, 0) == 0);
        CHECK(Parsed("-9223372036854775808", INT64_MIN, 0) == 0);
        CHECK(Parsed("922337203685477580700e-2", INT64_MAX, 0) == 0);
        CHECK(Parsed("922337203685477580.7", INT64_MAX, 1) == 0);
    }
    else if (!strcmp(mode, "precision"))
    {
        const char *texts[] = {"1e-10",  "1e19",    "9223372036854775808", "-9223372036854775809",
                               "1e1001", "1e-1001", "9223372036.854775808"};
        for (size_t index = 0U; index < sizeof texts / sizeof texts[0]; ++index)
        {
            UmiDecimal value = Amount(99, 1);
            CHECK(UmiDecimalParseScientificExact(texts[index], strlen(texts[index]), &value) ==
                  UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(value.coefficient == 99 && value.scale == 1U);
        }
    }
    else if (!strcmp(mode, "malformed"))
    {
        const char *texts[] = {"",   " ",   ".1",  "1.",  "NaN", "Infinity", "1,000",
                               "1e", "1e+", "1e-", "--1", " 1",  "1 ",       "1e2x"};
        for (size_t index = 0U; index < sizeof texts / sizeof texts[0]; ++index)
        {
            UmiDecimal value = Amount(99, 1);
            CHECK(UmiDecimalParseScientificExact(texts[index], strlen(texts[index]), &value) !=
                  UMI_STATUS_OK);
            CHECK(value.coefficient == 99 && value.scale == 1U);
        }
        UmiDecimal value = Amount(99, 1);
        CHECK(UmiDecimalParseScientificExact("1\0x", 3U, &value) == UMI_STATUS_PARSE_ERROR);
        CHECK(value.coefficient == 99 && value.scale == 1U);
    }
    else if (!strcmp(mode, "multiple"))
    {
        const struct
        {
            int64_t value, step;
            uint8_t valueScale, stepScale;
            bool aligned;
        } cases[] = {{425, 5, 2, 2, true},   {426, 5, 2, 2, false},   {1, 25, 0, 2, true},
                     {1250, 25, 3, 2, true}, {1251, 25, 3, 2, false}, {-1250, 25, 3, 2, true},
                     {0, 7, 9, 0, true},     {123, 3, 9, 9, true}};
        for (size_t index = 0U; index < sizeof cases / sizeof cases[0]; ++index)
        {
            bool aligned = !cases[index].aligned;
            CHECK(UmiDecimalIsMultiple(Amount(cases[index].value, cases[index].valueScale),
                                       Amount(cases[index].step, cases[index].stepScale),
                                       &aligned) == UMI_STATUS_OK);
            CHECK(aligned == cases[index].aligned);
        }
    }
    else if (!strcmp(mode, "wide"))
    {
        bool aligned = false;
        CHECK(UmiDecimalIsMultiple(Amount(INT64_MAX, 0), Amount(INT64_MAX, 9), &aligned) == UMI_STATUS_OK &&
              aligned);
        CHECK(UmiDecimalIsMultiple(Amount(INT64_MAX, 0), Amount(2, 9), &aligned) == UMI_STATUS_OK && aligned);
        CHECK(UmiDecimalIsMultiple(Amount(INT64_MIN, 0), Amount(3, 9), &aligned) == UMI_STATUS_OK &&
              !aligned);
        CHECK(UmiDecimalIsMultiple(Amount(INT64_MIN, 0), Amount(2, 0), &aligned) == UMI_STATUS_OK && aligned);
    }
    else if (!strcmp(mode, "invalid"))
    {
        bool aligned = true;
        UmiDecimal value = Amount(99, 1);
        CHECK(UmiDecimalIsMultiple(value, Amount(0, 0), &aligned) == UMI_STATUS_INVALID_ARGUMENT && aligned);
        CHECK(UmiDecimalIsMultiple(value, Amount(-1, 0), &aligned) == UMI_STATUS_INVALID_ARGUMENT && aligned);
        CHECK(UmiDecimalIsMultiple(Amount(1, 10), Amount(1, 0), &aligned) == UMI_STATUS_INVALID_ARGUMENT &&
              aligned);
        CHECK(UmiDecimalIsMultiple(value, Amount(1, 0), NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDecimalParseScientificExact(NULL, 1U, &value) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(value.coefficient == 99 && value.scale == 1U);
        CHECK(UmiDecimalParseScientificExact("1", 1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    }
    else
        return 2;
    return 0;
}
