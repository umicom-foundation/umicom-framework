/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_ascii_number.c
 * PURPOSE: Cover numeric boundaries and ambient-locale independence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "structured_form_fixture.h"
#include "umicom/base/ascii_number.h"
#include <float.h>
#include <locale.h>
#include <math.h>
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1];
  uint64_t u = 19U;
  int64_t i = 19;
  double value = 19.0;
  char text[128] = "unchanged";
  size_t length = 19U;
  if (strcmp(name, "unsigned") == 0) {
    CHECK(umi_ascii_uint64_parse("18446744073709551615", 20U, &u) == UMI_STATUS_OK &&
          u == UINT64_MAX);
    CHECK(umi_ascii_uint64_parse("0", 1U, &u) == UMI_STATUS_OK && u == 0U);
    CHECK(umi_ascii_uint64_parse("+1", 2U, &u) == UMI_STATUS_PARSE_ERROR && u == 0U);
    CHECK(umi_ascii_uint64_parse("-1", 2U, &u) == UMI_STATUS_PARSE_ERROR && u == 0U);
  } else if (strcmp(name, "signed") == 0) {
    CHECK(umi_ascii_int64_parse("-9223372036854775808", 20U, &i) == UMI_STATUS_OK &&
          i == INT64_MIN);
    CHECK(umi_ascii_int64_parse("9223372036854775807", 19U, &i) == UMI_STATUS_OK && i == INT64_MAX);
    CHECK(umi_ascii_int64_parse("+42", 3U, &i) == UMI_STATUS_OK && i == 42);
  } else if (strcmp(name, "overflow") == 0) {
    CHECK(umi_ascii_uint64_parse("18446744073709551616", 20U, &u) == UMI_STATUS_CAPACITY_EXCEEDED &&
          u == 19U);
    CHECK(umi_ascii_int64_parse("9223372036854775808", 19U, &i) == UMI_STATUS_CAPACITY_EXCEEDED &&
          i == 19);
    CHECK(umi_ascii_int64_parse("-9223372036854775809", 20U, &i) == UMI_STATUS_CAPACITY_EXCEEDED &&
          i == 19);
    CHECK(umi_ascii_double_parse("1e999", 5U, &value) == UMI_STATUS_CAPACITY_EXCEEDED &&
          value == 19.0);
    CHECK(umi_ascii_double_parse("1e-999", 6U, &value) == UMI_STATUS_CAPACITY_EXCEEDED &&
          value == 19.0);
  } else if (strcmp(name, "spelling") == 0) {
    const char *bad[] = {"",    "+",   "-",     " 3", "3 ", "3,5", "3kg",
                         "nan", "inf", "0x1p0", "3.", ".5", "1e",  "1e+"};
    for (size_t at = 0U; at < sizeof(bad) / sizeof(bad[0]); ++at)
      CHECK(umi_ascii_double_parse(bad[at], strlen(bad[at]), &value) == UMI_STATUS_PARSE_ERROR &&
            value == 19.0);
  } else if (strcmp(name, "decimal") == 0) {
    CHECK(umi_ascii_double_parse("+125.0e-2", 9U, &value) == UMI_STATUS_OK && value == 1.25);
    CHECK(umi_ascii_double_parse("-0.5", 4U, &value) == UMI_STATUS_OK && value == -0.5);
  } else if (strcmp(name, "roundtrip") == 0 || strcmp(name, "subnormal") == 0) {
    const double values[] = {0.1, 1.0, -13.25, DBL_MAX, DBL_MIN, DBL_TRUE_MIN};
    for (size_t at = 0U; at < sizeof(values) / sizeof(values[0]); ++at) {
      CHECK(umi_ascii_double_format(values[at], text, sizeof(text), &length) == UMI_STATUS_OK);
      CHECK(umi_ascii_double_parse(text, length, &value) == UMI_STATUS_OK && value == values[at]);
    }
  } else if (strcmp(name, "capacity") == 0) {
    CHECK(umi_ascii_double_format(123.5, text, 2U, &length) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(strcmp(text, "unchanged") == 0 && length == 19U);
    CHECK(umi_ascii_double_format(INFINITY, text, sizeof(text), &length) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(strcmp(text, "unchanged") == 0 && length == 19U);
  } else if (strcmp(name, "locale") == 0) {
    const char *old = setlocale(LC_NUMERIC, NULL);
    CHECK(old != NULL && strlen(old) < sizeof(text));
    strcpy(text, old);
    const char *choices[] = {"de_DE.UTF-8", "fr_FR.UTF-8", "German_Germany.1252",
                             "French_France.1252"};
    int selected = 0;
    for (size_t at = 0U; at < sizeof(choices) / sizeof(choices[0]); ++at)
      if (setlocale(LC_NUMERIC, choices[at]) != NULL) {
        selected = 1;
        break;
      }
    if (!selected) {
      (void)setlocale(LC_NUMERIC, text);
      fprintf(stderr, "SKIP: no comma-decimal locale installed\n");
      return 77;
    }
    char rendered[128];
    int good =
        umi_ascii_double_parse("1.5", 3U, &value) == UMI_STATUS_OK && value == 1.5 &&
        umi_ascii_double_format(value, rendered, sizeof(rendered), &length) == UMI_STATUS_OK &&
        strcmp(rendered, "1.5") == 0;
    CHECK(setlocale(LC_NUMERIC, text) != NULL);
    CHECK(good);
  } else if (strcmp(name, "embedded") == 0) {
    const char bad[] = {'1', '\0', '2'};
    CHECK(umi_ascii_int64_parse(bad, sizeof(bad), &i) == UMI_STATUS_PARSE_ERROR && i == 19);
    CHECK(umi_ascii_double_parse(bad, sizeof(bad), &value) == UMI_STATUS_PARSE_ERROR &&
          value == 19.0);
  } else if (strcmp(name, "bounded") == 0) {
    const char exact[] = {'4', '2'};
    CHECK(umi_ascii_int64_parse(exact, sizeof(exact), &i) == UMI_STATUS_OK && i == 42);
    CHECK(umi_ascii_int64_parse(exact, UMI_ASCII_NUMBER_MAX_BYTES + 1U, &i) ==
              UMI_STATUS_CAPACITY_EXCEEDED &&
          i == 42);
    CHECK(umi_ascii_int64_parse(NULL, 0U, &i) == UMI_STATUS_INVALID_ARGUMENT);
  } else if (strcmp(name, "negative-zero") == 0) {
    /* Some Windows math headers route signbit through a float helper.
     * Preserve that assertion for review; copysign keeps this double-valued
     * contract and also requires the parsed value to remain exactly zero. */
#if 0
    CHECK(umi_ascii_double_parse("-0", 2U, &value) == UMI_STATUS_OK && signbit(value));
#endif
    CHECK(umi_ascii_double_parse("-0", 2U, &value) == UMI_STATUS_OK && value == 0.0 &&
          copysign(1.0, value) < 0.0);
    CHECK(umi_ascii_double_format(value, text, sizeof(text), &length) == UMI_STATUS_OK);
    value = 1.0;
    /* Some Windows math headers route signbit through a float helper.
     * Preserve that assertion for review; copysign keeps this double-valued
     * contract and also requires the parsed value to remain exactly zero. */
#if 0
    CHECK(umi_ascii_double_parse(text, length, &value) == UMI_STATUS_OK && signbit(value));
#endif
    CHECK(umi_ascii_double_parse(text, length, &value) == UMI_STATUS_OK && value == 0.0 &&
          copysign(1.0, value) < 0.0);
  } else
    CHECK(0);
  return 0;
}
