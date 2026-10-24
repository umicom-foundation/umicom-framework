/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_form_temporal.c
 * PURPOSE: Exercise bounded calendar and timezone parsing without local settings.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "structured_form_fixture.h"
#include "umicom/application/experience/form_temporal.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1];
  UmiFormTimestamp parsed = {0}, before = {0};
  memset(&before, 0x5a, sizeof(before));
  parsed = before;
  if (strcmp(name, "date") == 0) {
    const char *good[] = {"2000-02-29", "2024-02-29", "0001-01-01", "9999-12-31"};
    const char *bad[] = {"1900-02-29", "2023-02-29", "2024-04-31", "0000-01-01", "2024-00-01",
                         "2024-01-00", "2024-1-01",  "2024-13-01", "2024-01-32"};
    for (size_t i = 0U; i < sizeof(good) / sizeof(good[0]); ++i)
      CHECK(umi_form_date_validate(good[i], strlen(good[i])) == UMI_STATUS_OK);
    for (size_t i = 0U; i < sizeof(bad) / sizeof(bad[0]); ++i)
      CHECK(umi_form_date_validate(bad[i], strlen(bad[i])) == UMI_STATUS_PARSE_ERROR);
  } else if (strcmp(name, "offset") == 0) {
    const char *s = "2024-02-29T23:59:59.123456789-03:30";
    CHECK(umi_form_timestamp_parse(s, strlen(s), &parsed) == UMI_STATUS_OK);
    CHECK(parsed.year == 2024U && parsed.month == 2U && parsed.day == 29U && parsed.hour == 23U &&
          parsed.nanosecond == 123456789U && parsed.offset_minutes == -210);
    s = "2024-02-29T00:00:00.1+14:00";
    CHECK(umi_form_timestamp_parse(s, strlen(s), &parsed) == UMI_STATUS_OK);
    CHECK(parsed.offset_minutes == 840 && parsed.nanosecond == 100000000U);
    s = "2024-02-29T00:00:00Z";
    CHECK(umi_form_timestamp_parse(s, strlen(s), &parsed) == UMI_STATUS_OK);
    CHECK(parsed.offset_minutes == 0 && parsed.nanosecond == 0U);
  } else if (strcmp(name, "ambiguous") == 0) {
    const char *bad[] = {"2024-02-29T12:00:00",       "2024-02-29T12:00:00-00:00",
                         "2024-02-29T12:00:00+14:01", "2024-02-29T24:00:00Z",
                         "2024-02-29T23:60:00Z",      "2024-02-29T23:59:60Z",
                         "2024-02-29T12:00:00.Z",     "2024-02-29T12:00:00.1234567890Z",
                         "2024-02-29 12:00:00Z",      "2024-02-29T12:00:00z",
                         "2024-02-29T12:00:00Ztail"};
    for (size_t i = 0U; i < sizeof(bad) / sizeof(bad[0]); ++i) {
      CHECK(umi_form_timestamp_parse(bad[i], strlen(bad[i]), &parsed) == UMI_STATUS_PARSE_ERROR);
      CHECK(memcmp(&parsed, &before, sizeof(parsed)) == 0);
    }
  } else if (strcmp(name, "bounded") == 0) {
    const char text[20] = {'2', '0', '2', '4', '-', '0', '2', '-', '2', '9',
                           'T', '0', '0', ':', '0', '0', ':', '0', '0', 'Z'};
    for (size_t n = 0U; n < 20U; ++n) {
      CHECK(umi_form_timestamp_parse(text, n, &parsed) == UMI_STATUS_PARSE_ERROR);
      CHECK(memcmp(&parsed, &before, sizeof(parsed)) == 0);
    }
    CHECK(umi_form_timestamp_parse(text, sizeof(text), &parsed) == UMI_STATUS_OK);
    CHECK(umi_form_timestamp_parse(NULL, 20U, &parsed) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_form_timestamp_parse(text, 20U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
  } else
    CHECK(0);
  return 0;
}
