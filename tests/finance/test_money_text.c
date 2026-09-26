/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance/test_money_text.c
 *
 * PURPOSE:
 *   Check exact signed money, every public scale and failure atomicity.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/money_text.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); return 1; } } while (0)
static UmiMoney Money(int64_t minor, uint8_t scale)
{
    UmiMoney money = {0}; money.minor_units = minor; money.scale = scale;
    memcpy(money.currency.code, "GBP", 4U); return money;
}
static int Expect(int64_t minor, uint8_t scale, const char *expected)
{
    UmiMoney money = Money(minor, scale);
    char actual[UMI_MONEY_TEXT_CAPACITY]; size_t required = 0U;
    CHECK(UmiMoneyTextFormat(&money, actual, sizeof actual, &required) == UMI_STATUS_OK);
    CHECK(strcmp(actual, expected) == 0); CHECK(required == strlen(expected) + 1U);
    return 0;
}
static int Amounts(void)
{
    CHECK(Expect(0, 0, "GBP 0") == 0); CHECK(Expect(0, 2, "GBP 0.00") == 0);
    CHECK(Expect(1, 2, "GBP 0.01") == 0); CHECK(Expect(-1, 2, "GBP -0.01") == 0);
    CHECK(Expect(-99, 2, "GBP -0.99") == 0); CHECK(Expect(-100, 2, "GBP -1.00") == 0);
    CHECK(Expect(-101, 2, "GBP -1.01") == 0); CHECK(Expect(12005, 2, "GBP 120.05") == 0);
    CHECK(Expect(12345, 0, "GBP 12345") == 0); CHECK(Expect(-12345, 0, "GBP -12345") == 0);
    CHECK(Expect(12005, 3, "GBP 12.005") == 0); CHECK(Expect(1, 18, "GBP 0.000000000000000001") == 0);
    return 0;
}
static int Extremes(void)
{
    CHECK(Expect(INT64_MIN, 0, "GBP -9223372036854775808") == 0);
    CHECK(Expect(INT64_MAX, 0, "GBP 9223372036854775807") == 0);
    CHECK(Expect(INT64_MIN, 2, "GBP -92233720368547758.08") == 0);
    CHECK(Expect(INT64_MAX, 2, "GBP 92233720368547758.07") == 0);
    CHECK(Expect(INT64_MIN, 18, "GBP -9.223372036854775808") == 0);
    CHECK(Expect(INT64_MIN, 19, "GBP -0.9223372036854775808") == 0);
    CHECK(Expect(INT64_MIN, 20, "GBP -0.09223372036854775808") == 0);
    return 0;
}
static int AllScales(void)
{
    for (unsigned scale = 0U; scale <= 255U; ++scale) {
        char expected[UMI_MONEY_TEXT_CAPACITY] = "GBP -";
        size_t length = 5U;
        if (scale != 0U) {
            expected[length++] = '0'; expected[length++] = '.';
            for (unsigned i = 1U; i < scale; ++i) expected[length++] = '0';
        }
        expected[length++] = '1'; expected[length] = '\0';
        CHECK(Expect(-1, (uint8_t)scale, expected) == 0);
        /* Zero never acquires a negative sign or loses requested precision. */
        memcpy(expected, "GBP ", 4U); length = 4U; expected[length++] = '0';
        if (scale != 0U) expected[length++] = '.';
        for (unsigned i = 0U; i < scale; ++i) expected[length++] = '0';
        expected[length] = '\0'; CHECK(Expect(0, (uint8_t)scale, expected) == 0);
    }
    return 0;
}
static int Capacity(void)
{
    UmiMoney money = Money(INT64_MIN, 255U);
    char text[UMI_MONEY_TEXT_CAPACITY], saved[UMI_MONEY_TEXT_CAPACITY]; size_t required = 0U;
    memset(text, 'x', sizeof text); memcpy(saved, text, sizeof saved);
    CHECK(UmiMoneyTextFormat(&money, NULL, 0U, &required) == UMI_STATUS_OK);
    CHECK(required == 263U);
    for (size_t capacity = 0U; capacity < required; ++capacity) {
        size_t needed = 0U;
        CHECK(UmiMoneyTextFormat(&money, text, capacity, &needed) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(needed == required); CHECK(memcmp(text, saved, sizeof text) == 0);
    }
    CHECK(UmiMoneyTextFormat(&money, text, required, NULL) == UMI_STATUS_OK);
    CHECK(text[required - 1U] == '\0'); CHECK(text[required] == 'x');
    return 0;
}
static int Invalid(void)
{
    UmiMoney money = Money(125, 2); char text[32] = "unchanged"; size_t required = 99U;
    static const char bad[][4] = {"gbp", "G1P", "GB", "", {'G','B','P','X'}};
    for (size_t index = 0U; index < sizeof bad / sizeof bad[0]; ++index) {
        memcpy(money.currency.code, bad[index], 4U);
        CHECK(UmiMoneyTextFormat(&money, text, sizeof text, &required) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(required == 0U && strcmp(text, "unchanged") == 0);
    }
    return 0;
}
static int Arguments(void)
{
    UmiMoney money = Money(125, 2); char text[32] = "unchanged"; size_t required = 99U;
    CHECK(UmiMoneyTextFormat(NULL, text, sizeof text, &required) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(required == 0U && strcmp(text, "unchanged") == 0);
    CHECK(UmiMoneyTextFormat(&money, NULL, 1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiMoneyTextFormat(&money, NULL, 0U, NULL) == UMI_STATUS_OK);
    CHECK(UmiMoneyTextFormat(&money, text, sizeof text, NULL) == UMI_STATUS_OK);
    CHECK(strcmp(text, "GBP 1.25") == 0); return 0;
}
static int Overlapping(void)
{
    union { UmiMoney money; char text[UMI_MONEY_TEXT_CAPACITY]; } shared;
    shared.money = Money(-1, 2U);
    CHECK(UmiMoneyTextFormat(&shared.money, shared.text, sizeof shared.text, NULL) == UMI_STATUS_OK);
    CHECK(strcmp(shared.text, "GBP -0.01") == 0); return 0;
}
int main(int argc, char **argv)
{
    int result = 2;
    if (argc == 4 && strcmp(argv[1], "--format") == 0) {
        char *end; intmax_t minor; unsigned long scale;
        UmiMoney money; char text[UMI_MONEY_TEXT_CAPACITY];
        errno = 0; minor = strtoimax(argv[2], &end, 10);
        if (errno != 0 || end == argv[2] || *end != '\0' || minor < INT64_MIN || minor > INT64_MAX) return 2;
        errno = 0; scale = strtoul(argv[3], &end, 10);
        if (errno != 0 || end == argv[3] || *end != '\0' || scale > 255UL) return 2;
        money = Money((int64_t)minor, (uint8_t)scale);
        if (UmiMoneyTextFormat(&money, text, sizeof text, NULL) != UMI_STATUS_OK) return 1;
        puts(text); return 0;
    }
    if (argc != 2) return 2;
    if (strcmp(argv[1], "amounts") == 0) result = Amounts();
    else if (strcmp(argv[1], "extremes") == 0) result = Extremes();
    else if (strcmp(argv[1], "all_scales") == 0) result = AllScales();
    else if (strcmp(argv[1], "capacity") == 0) result = Capacity();
    else if (strcmp(argv[1], "invalid") == 0) result = Invalid();
    else if (strcmp(argv[1], "arguments") == 0) result = Arguments();
    else if (strcmp(argv[1], "overlapping") == 0) result = Overlapping();
    if (result == 0) printf("PASS money_text.%s\n", argv[1]);
    return result;
}
