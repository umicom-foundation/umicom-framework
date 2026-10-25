/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_report_view/test_filter.c
 *
 * PURPOSE:
 *   Check outcome masks, deterministic selection, Unicode admission and large saved reports.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include <limits.h>
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1],
             *names[] = {"default",       "all",           "passed",       "failed",
                         "timeout",       "skipped",       "disabled",     "not-run",
                         "none",          "name",          "name-order",   "duration-order",
                         "outcome-order", "copied-filter", "invalid-mask", "invalid-order",
                         "invalid-utf8",  "unterminated",  "null",         "empty",
                         "large",         "tie-order"};
  CHECK(Known(name, names, sizeof names / sizeof names[0]));
  UmiCtestReport *report = !strcmp(name, "empty")       ? Many(0)
                           : !strcmp(name, "large")     ? Many(100000U)
                           : !strcmp(name, "tie-order") ? Many(300U)
                                                        : Mixed();
  UmiCtestReportFilter filter;
  UmiCtestReportFilterInit(&filter);
  const char *outcomeNames[] = {"passed", "failed", "timeout", "skipped", "disabled", "not-run"};
  for (unsigned i = 0; i < 6U; ++i)
    if (!strcmp(name, outcomeNames[i]))
      filter.outcomes = 1U << i;
  if (!strcmp(name, "all"))
    filter.outcomes = UMI_CTEST_VIEW_ALL;
  if (!strcmp(name, "none"))
    filter.outcomes = 0U;
  if (!strcmp(name, "name"))
    strcpy(filter.name_contains, "FAIL");
  if (!strcmp(name, "name-order"))
    filter.order = UMI_CTEST_REPORT_NAME_ORDER;
  if (!strcmp(name, "duration-order") || !strcmp(name, "tie-order"))
    filter.order = UMI_CTEST_REPORT_LONGEST_FIRST;
  if (!strcmp(name, "outcome-order"))
    filter.order = UMI_CTEST_REPORT_OUTCOME_ORDER;
  if (!strcmp(name, "invalid-mask"))
    filter.outcomes = 128U;
  if (!strcmp(name, "invalid-order"))
    filter.order = (UmiCtestReportOrder)99;
  if (!strcmp(name, "invalid-utf8"))
    strcpy(filter.name_contains, "\xff");
  if (!strcmp(name, "unterminated"))
    memset(filter.name_contains, 'a', sizeof filter.name_contains);
  UmiCtestReportView *view = NULL;
  UmiStatus status = UmiCtestReportViewCreate(report, &filter, &view);
  if (!strncmp(name, "invalid-", 8U) || !strcmp(name, "unterminated")) {
    CHECK(status == UMI_STATUS_INVALID_ARGUMENT && view == NULL);
    if (!strcmp(name, "invalid-mask")) {
      /* Check every representable flag, including the top unsigned bit. This
       * keeps rejection independent of the enum constant's promoted type. */
      unsigned bit = 1U;
      do {
        filter.outcomes = bit;
        UmiStatus expected = bit <= (unsigned)UMI_CTEST_VIEW_ALL
                                 ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
        CHECK(UmiCtestReportFilterValidate(&filter) == expected);
        if (expected == UMI_STATUS_INVALID_ARGUMENT) {
          filter.outcomes |= (unsigned)UMI_CTEST_VIEW_ALL;
          CHECK(UmiCtestReportViewCreate(report, &filter, &view) ==
                UMI_STATUS_INVALID_ARGUMENT && view == NULL);
        }
        bit <<= 1U;
      } while (bit != 0U);
      filter.outcomes = UINT_MAX;
      CHECK(UmiCtestReportFilterValidate(&filter) == UMI_STATUS_INVALID_ARGUMENT);
      filter.outcomes = UMI_CTEST_VIEW_ALL;
      OK(UmiCtestReportFilterValidate(&filter));
      filter.outcomes = 0U;
      OK(UmiCtestReportFilterValidate(&filter));
    }
    goto done;
  }
  OK(status);
  UmiCtestReportViewSummary summary;
  OK(UmiCtestReportViewRead(view, &summary));
  size_t expected = 5U;
  if (!strcmp(name, "all"))
    expected = 6U;
  else if (!strcmp(name, "empty") || !strcmp(name, "none"))
    expected = 0U;
  else if (!strcmp(name, "large"))
    expected = 100000U;
  else if (!strcmp(name, "tie-order"))
    expected = 300U;
  else if (!strcmp(name, "name"))
    expected = 1U;
  for (size_t i = 0; i < 6U; ++i)
    if (!strcmp(name, outcomeNames[i]))
      expected = 1U;
  CHECK(summary.matches == expected);
  if (!strcmp(name, "copied-filter")) {
    memset(&filter, 0, sizeof filter);
    CHECK(UmiCtestReportViewAt(view, 0) == UmiCtestReportEntryAt(report, 1));
  }
  if (!strcmp(name, "name"))
    CHECK(!strcmp(UmiCtestReportViewAt(view, 0)->name, "b.fail"));
  if (!strcmp(name, "name-order"))
    CHECK(!strcmp(UmiCtestReportViewAt(view, 0)->name, "a.missing"));
  if (!strcmp(name, "duration-order"))
    CHECK(!strcmp(UmiCtestReportViewAt(view, 0)->name, "d.timeout"));
  if (!strcmp(name, "outcome-order"))
    for (size_t i = 1; i < expected; ++i)
      CHECK(UmiCtestReportEntryOutcome(UmiCtestReportViewAt(view, i - 1)) <
            UmiCtestReportEntryOutcome(UmiCtestReportViewAt(view, i)));
  if (!strcmp(name, "tie-order")) {
    for (size_t i = 1; i < expected; ++i) {
      const UmiCtestReportEntry *a = UmiCtestReportViewAt(view, i - 1),
                                *b = UmiCtestReportViewAt(view, i);
      CHECK(a->result.duration_ms >= b->result.duration_ms);
      if (a->result.duration_ms == b->result.duration_ms)
        CHECK(strcmp(a->name, b->name) > 0);
    }
  }
  if (!strcmp(name, "null")) {
    UmiCtestReportView *bad = view;
    CHECK(UmiCtestReportViewCreate(NULL, &filter, &bad) == UMI_STATUS_INVALID_ARGUMENT &&
          bad == NULL);
    CHECK(UmiCtestReportViewCreate(report, NULL, &bad) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiCtestReportViewCreate(report, &filter, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiCtestReportViewRead(view, NULL) == UMI_STATUS_INVALID_ARGUMENT);
  }
  CHECK(UmiCtestReportViewAt(view, expected) == NULL);
  CHECK(UmiCtestReportViewAt(NULL, 0) == NULL);
  UmiCtestReportViewDestroy(view);
done:
  UmiCtestReportDestroy(report);
  return 0;
}
