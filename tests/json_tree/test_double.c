/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/json_tree/test_double.c
 *
 * PURPOSE:
 *   Verify finite JSON decimal interpretation, exact integer isolation and unchanged failure outputs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/language_runtime/json_tree.h"
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); failed = 1; goto cleanup; \
} } while (0)

static UmiStatus Parse(const char *text, UmiJsonTree **out)
{
    return UmiJsonTreeCreate(text, strlen(text), NULL, NULL, out);
}

/* A standalone test process may change its locale to challenge the service.
 * Restore it on every path; unavailable comma-decimal coverage is a skip. */
static int LocaleCase(void)
{
    char saved[256];
    const char *current = setlocale(LC_NUMERIC, NULL);
    if (current == NULL || strlen(current) >= sizeof(saved))
        return 1;
    strcpy(saved, current);
    const char *candidates[] = {"de_DE.UTF-8", "fr_FR.UTF-8",
        "German_Germany.1252", "French_France.1252"};
    int selected = 0;
    for (size_t i = 0U; i < sizeof(candidates) / sizeof(candidates[0]); ++i)
    {
        if (setlocale(LC_NUMERIC, candidates[i]) != NULL &&
            strcmp(localeconv()->decimal_point, ",") == 0)
        {
            selected = 1;
            break;
        }
    }
    if (!selected)
    {
        int restored = setlocale(LC_NUMERIC, saved) != NULL;
        fprintf(stderr, "SKIP: no comma-decimal locale installed\n");
        return restored ? 77 : 1;
    }
    UmiJsonTree *tree = NULL;
    double value = 19.25;
    int good = Parse("1.5", &tree) == UMI_STATUS_OK &&
        UmiJsonTreeDouble(tree, 0, &value) == UMI_STATUS_OK && value == 1.5 &&
        strcmp(localeconv()->decimal_point, ",") == 0;
    UmiJsonTreeDestroy(tree);
    int restored = setlocale(LC_NUMERIC, saved) != NULL;
    return good && restored ? 0 : 1;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    if (strcmp(mode, "locale") == 0)
        return LocaleCase();
    int failed = 0;
    UmiJsonTree *tree = NULL, *other = NULL;
    double value = 19.25;
    int node = -1;
    if (strcmp(mode, "decimal") == 0 || strcmp(mode, "exponent") == 0 ||
        strcmp(mode, "integer-spelling") == 0 || strcmp(mode, "negative-zero") == 0)
    {
        const char *text = strcmp(mode, "decimal") == 0 ? "-12.5" :
            strcmp(mode, "exponent") == 0 ? "1.25e+2" :
            strcmp(mode, "integer-spelling") == 0 ? "42" : "-0.0";
        double expected = strcmp(mode, "decimal") == 0 ? -12.5 :
            strcmp(mode, "exponent") == 0 ? 125.0 :
            strcmp(mode, "integer-spelling") == 0 ? 42.0 : 0.0;
        CHECK(Parse(text, &tree) == UMI_STATUS_OK);
        CHECK(UmiJsonTreeDouble(tree, 0, &value) == UMI_STATUS_OK && value == expected);
        if (strcmp(mode, "negative-zero") == 0)
            CHECK(copysign(1.0, value) < 0.0);
    }
    else if (strcmp(mode, "largest") == 0 || strcmp(mode, "normal-minimum") == 0 ||
             strcmp(mode, "subnormal") == 0)
    {
        if (DBL_MANT_DIG != 53 || DBL_MAX_EXP != 1024)
        {
            fprintf(stderr, "SKIP: fixture requires IEEE binary64 double\n");
            return 77;
        }
        const char *text = strcmp(mode, "largest") == 0 ? "1.7976931348623157e308" :
            strcmp(mode, "normal-minimum") == 0 ? "2.2250738585072014e-308" :
            "4.9406564584124654e-324";
        double expected = strcmp(mode, "largest") == 0 ? DBL_MAX :
            strcmp(mode, "normal-minimum") == 0 ? DBL_MIN : DBL_TRUE_MIN;
        CHECK(Parse(text, &tree) == UMI_STATUS_OK);
        CHECK(UmiJsonTreeDouble(tree, 0, &value) == UMI_STATUS_OK && value == expected);
    }
    else if (strcmp(mode, "overflow") == 0 || strcmp(mode, "underflow") == 0 ||
             strcmp(mode, "negative-overflow") == 0)
    {
        const char *text = strcmp(mode, "overflow") == 0 ? "1e999999" :
            strcmp(mode, "underflow") == 0 ? "1e-999999" : "-1e999999";
        CHECK(Parse(text, &tree) == UMI_STATUS_OK);
        CHECK(UmiJsonTreeDouble(tree, 0, &value) == UMI_STATUS_CAPACITY_EXCEEDED &&
              value == 19.25);
    }
    else if (strcmp(mode, "wrong-primitives") == 0 || strcmp(mode, "wrong-containers") == 0 ||
             strcmp(mode, "string-number") == 0)
    {
        const char *values[] = {"true", "false", "null", "[]", "{}", "\"7.5\""};
        size_t begin = strcmp(mode, "wrong-primitives") == 0 ? 0U :
            strcmp(mode, "wrong-containers") == 0 ? 3U : 5U;
        size_t end = begin == 0U ? 3U : begin == 3U ? 5U : 6U;
        for (size_t i = begin; i < end; ++i)
        {
            CHECK(Parse(values[i], &tree) == UMI_STATUS_OK);
            CHECK(UmiJsonTreeDouble(tree, 0, &value) == UMI_STATUS_PARSE_ERROR && value == 19.25);
            UmiJsonTreeDestroy(tree);
            tree = NULL;
        }
    }
    else if (strcmp(mode, "invalid-handles") == 0)
    {
        CHECK(Parse("1", &tree) == UMI_STATUS_OK);
        CHECK(UmiJsonTreeDouble(NULL, 0, &value) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiJsonTreeDouble(tree, -1, &value) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiJsonTreeDouble(tree, INT_MAX, &value) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiJsonTreeDouble(tree, 1, &value) == UMI_STATUS_INVALID_ARGUMENT && value == 19.25);
    }
    else if (strcmp(mode, "null-output") == 0)
    {
        CHECK(Parse("false", &tree) == UMI_STATUS_OK);
        CHECK(UmiJsonTreeDouble(tree, 0, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    }
    else if (strcmp(mode, "spelling-limit") == 0 || strcmp(mode, "spelling-too-long") == 0)
    {
        char text[513];
        /* Independent documented boundary: JSON accepts either spelling, while
         * numeric conversion limits work to 511 bytes even for a zero value. */
        size_t length = strcmp(mode, "spelling-limit") == 0 ? 511U : 512U;
        memset(text, '0', length);
        text[1] = '.';
        text[length] = '\0';
        CHECK(Parse(text, &tree) == UMI_STATUS_OK);
        UmiStatus status = UmiJsonTreeDouble(tree, 0, &value);
        CHECK(length == 511U ? status == UMI_STATUS_OK && value == 0.0 :
            status == UMI_STATUS_CAPACITY_EXCEEDED && value == 19.25);
    }
    else if (strcmp(mode, "owned-span") == 0)
    {
        char bytes[] = {'7', '.', '5'};
        CHECK(UmiJsonTreeCreate(bytes, sizeof(bytes), NULL, NULL, &tree) == UMI_STATUS_OK);
        memset(bytes, '?', sizeof(bytes));
        const char *span = NULL;
        size_t length = 0U;
        CHECK(UmiJsonTreeSourceSpan(tree, 0, &span, &length) == UMI_STATUS_OK);
        CHECK(length == 3U && memcmp(span, "7.5", 3U) == 0);
        CHECK(UmiJsonTreeDouble(tree, 0, &value) == UMI_STATUS_OK && value == 7.5);
        CHECK(length == 3U && memcmp(span, "7.5", 3U) == 0);
    }
    else if (strcmp(mode, "member-range") == 0)
    {
        CHECK(Parse("{\"default\":7.5,\"min\":0,\"max\":30.0}", &tree) == UMI_STATUS_OK);
        const char *names[] = {"default", "min", "max"};
        const double expected[] = {7.5, 0.0, 30.0};
        for (size_t i = 0U; i < 3U; ++i)
        {
            CHECK(UmiJsonTreeMember(tree, 0, names[i], &node) == UMI_STATUS_OK);
            CHECK(UmiJsonTreeDouble(tree, node, &value) == UMI_STATUS_OK && value == expected[i]);
        }
    }
    else if (strcmp(mode, "exact-integer-preserved") == 0)
    {
        int64_t exact = 0;
        CHECK(Parse("9007199254740993", &tree) == UMI_STATUS_OK);
        CHECK(UmiJsonTreeInteger(tree, 0, &exact) == UMI_STATUS_OK &&
              exact == INT64_C(9007199254740993));
        CHECK(UmiJsonTreeDouble(tree, 0, &value) == UMI_STATUS_OK);
        CHECK(UmiJsonTreeInteger(tree, 0, &exact) == UMI_STATUS_OK &&
              exact == INT64_C(9007199254740993));
        UmiJsonTreeDestroy(tree);
        tree = NULL;
        CHECK(Parse("1.0", &tree) == UMI_STATUS_OK);
        CHECK(UmiJsonTreeInteger(tree, 0, &exact) == UMI_STATUS_PARSE_ERROR &&
              exact == INT64_C(9007199254740993));
    }
    else if (strcmp(mode, "comparison-preserved") == 0)
    {
        int equal = 1;
        CHECK(Parse("1", &tree) == UMI_STATUS_OK && Parse("1.0", &other) == UMI_STATUS_OK);
        CHECK(UmiJsonTreeDouble(tree, 0, &value) == UMI_STATUS_OK && value == 1.0);
        CHECK(UmiJsonTreeDouble(other, 0, &value) == UMI_STATUS_OK && value == 1.0);
        CHECK(UmiJsonTreeValuesEqual(tree, 0, other, 0, NULL, &equal) == UMI_STATUS_OK && !equal);
    }
    else if (strcmp(mode, "json-grammar") == 0)
    {
        const char *bad[] = {"+1", "01", ".5", "1.", "NaN", "Infinity", "0x1p0", "1,5"};
        for (size_t i = 0U; i < sizeof(bad) / sizeof(bad[0]); ++i)
            CHECK(Parse(bad[i], &tree) == UMI_STATUS_PARSE_ERROR && tree == NULL);
    }
    else
        CHECK(0);
cleanup:
    UmiJsonTreeDestroy(other);
    UmiJsonTreeDestroy(tree);
    return failed;
}
