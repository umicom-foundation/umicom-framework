/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_feed/test_csv_issue_locations.c
 *
 * PURPOSE:
 *   Distinguish unlocalised CSV admission errors from decoded field errors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "csv_fixture.h"
#include "fixture.h"
#include "umicom/bank_feed/csv_mapping.h"

/* Test both import paths with the same bytes. A text admission failure must
 * not invent a field coordinate; a decoded invalid date must retain one. */
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *names[] = {"normal-header-control", "mapped-header-control",
                         "normal-data-control",   "mapped-data-control",
                         "normal-invalid-utf8",   "mapped-invalid-utf8",
                         "normal-embedded-nul",   "mapped-embedded-nul",
                         "normal-date",           "mapped-date"};
  size_t selected = sizeof names / sizeof names[0];
  for (size_t i = 0; i < sizeof names / sizeof names[0]; ++i)
    if (strcmp(argv[1], names[i]) == 0)
      selected = i;
  CHECK(selected < sizeof names / sizeof names[0]);
  char text[] = BANK_CSV_HEADER BANK_CSV_CREDIT;
  size_t length = sizeof text - 1U;
  char *information = strstr(text, "Example credit");
  CHECK(information != NULL);
  switch (selected / 2U) {
  case 0:
    text[0] = '\1';
    break;
  case 1:
    information[3] = '\177';
    break;
  case 2:
    information[3] = (char)0xc0;
    information[4] = (char)0xaf;
    break;
  case 3:
    information[3] = '\0';
    break;
  case 4: {
    char *date = strstr(text, "2026-10-12");
    CHECK(date != NULL);
    memcpy(date, "2026-02-30", 10U);
    break;
  }
  default:
    CHECK(0);
  }
  UmiBankFeedCsvIssue issue = {99U, 99U};
  UmiBankFeed *feed = (UmiBankFeed *)1;
  UmiStatus status;
  if ((selected & 1U) != 0U) {
    UmiBankFeedCsvMapping mapping;
    UmiBankFeedCsvMappingInit(&mapping);
    status = UmiBankFeedCsvMappedCreate(text, length, &mapping, &feed, &issue);
  } else {
    status = UmiBankFeedCsvCreate(text, length, &feed, &issue);
  }
  CHECK(status == UMI_STATUS_PARSE_ERROR && feed == NULL);
  CHECK(issue.record == (selected >= 8U ? 2U : 0U));
  CHECK(issue.column == (selected >= 8U ? 5U : 0U));
  return 0;
}
