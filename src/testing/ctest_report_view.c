/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/ctest_report_view.c
 *
 * PURPOSE:
 *   Select saved outcomes with deterministic ordering and no duplication of diagnostic payloads.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "ctest_report_view_private.h"
#include <stdlib.h>
#include <string.h>
unsigned UmiCtestReportEntryOutcome(const UmiCtestReportEntry *entry) {
  if (entry == NULL)
    return 0U;
  if (entry->disabled)
    return UMI_CTEST_VIEW_DISABLED;
  switch (entry->result.state) {
  case UMI_TEST_STATE_PASSED:
    return UMI_CTEST_VIEW_PASSED;
  case UMI_TEST_STATE_FAILED:
    return UMI_CTEST_VIEW_FAILED;
  case UMI_TEST_STATE_TIMED_OUT:
    return UMI_CTEST_VIEW_TIMED_OUT;
  case UMI_TEST_STATE_SKIPPED:
    return UMI_CTEST_VIEW_SKIPPED;
  case UMI_TEST_STATE_NOT_RUN:
    return UMI_CTEST_VIEW_NOT_RUN;
  default:
    return 0U;
  }
}
const char *UmiCtestReportOutcomeText(unsigned outcome) {
  switch (outcome) {
  case UMI_CTEST_VIEW_PASSED:
    return "passed";
  case UMI_CTEST_VIEW_FAILED:
    return "failed";
  case UMI_CTEST_VIEW_TIMED_OUT:
    return "timed out";
  case UMI_CTEST_VIEW_SKIPPED:
    return "skipped";
  case UMI_CTEST_VIEW_DISABLED:
    return "disabled";
  case UMI_CTEST_VIEW_NOT_RUN:
    return "not run";
  default:
    return "unknown";
  }
}
void UmiCtestReportFilterInit(UmiCtestReportFilter *filter) {
  if (filter != NULL) {
    memset(filter, 0, sizeof *filter);
    filter->outcomes = UMI_CTEST_VIEW_ALL & ~UMI_CTEST_VIEW_PASSED;
  }
}
static unsigned char Fold(unsigned char value) {
  return value >= 'A' && value <= 'Z' ? (unsigned char)(value + ('a' - 'A')) : value;
}
static bool Contains(const char *text, const char *needle) {
  if (needle[0] == '\0')
    return true;
  for (size_t start = 0U; text[start] != '\0'; ++start) {
    size_t i = 0U;
    while (needle[i] != '\0' && text[start + i] != '\0' &&
           Fold((unsigned char)text[start + i]) == Fold((unsigned char)needle[i]))
      ++i;
    if (needle[i] == '\0')
      return true;
  }
  return false;
}
static int SourceOrder(const ReportViewRow *a, const ReportViewRow *b) {
  return (a->source_index > b->source_index) - (a->source_index < b->source_index);
}
static int Names(const void *left, const void *right) {
  const ReportViewRow *a = left, *b = right;
  int value = strcmp(a->entry->name, b->entry->name);
  return value ? value : SourceOrder(a, b);
}
static int Durations(const void *left, const void *right) {
  const ReportViewRow *a = left, *b = right;
  uint64_t x = a->entry->result.duration_ms, y = b->entry->result.duration_ms;
  return x != y ? (x < y ? 1 : -1) : SourceOrder(a, b);
}
static int Outcomes(const void *left, const void *right) {
  const ReportViewRow *a = left, *b = right;
  unsigned x = UmiCtestReportEntryOutcome(a->entry), y = UmiCtestReportEntryOutcome(b->entry);
  return x != y ? (x < y ? -1 : 1) : SourceOrder(a, b);
}
void UmiCtestReportViewDestroy(UmiCtestReportView *view) {
  if (view != NULL) {
    free(view->rows);
    free(view);
  }
}
UmiStatus UmiCtestReportViewCreate(const UmiCtestReport *report, const UmiCtestReportFilter *filter,
                                   UmiCtestReportView **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = NULL;
  if (report == NULL || UmiCtestReportFilterValidate(filter) != UMI_STATUS_OK)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiCtestReportSummary summary;
  UmiStatus status = UmiCtestReportReadSummary(report, &summary);
  if (status != UMI_STATUS_OK)
    return status;
  if (summary.tests > UMI_CTEST_COLLECTION_MAX_TESTS ||
      summary.tests > SIZE_MAX / sizeof(ReportViewRow))
    return UMI_STATUS_CAPACITY_EXCEEDED;
  UmiCtestReportView *view = calloc(1U, sizeof *view);
  if (view == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  view->summary.source = summary;
  if (summary.tests != 0U) {
    view->rows = calloc(summary.tests, sizeof *view->rows);
    if (view->rows == NULL) {
      free(view);
      return UMI_STATUS_OUT_OF_MEMORY;
    }
  }
  for (size_t i = 0U; i < summary.tests; ++i) {
    const UmiCtestReportEntry *entry = UmiCtestReportEntryAt(report, i);
    if (entry == NULL || UmiCtestReportEntryOutcome(entry) == 0U) {
      UmiCtestReportViewDestroy(view);
      return UMI_STATUS_INVALID_STATE;
    }
    if ((filter->outcomes & UmiCtestReportEntryOutcome(entry)) != 0U &&
        Contains(entry->name, filter->name_contains))
      view->rows[view->summary.matches++] = (ReportViewRow){entry, i};
  }
  if (view->summary.matches > 1U && filter->order != UMI_CTEST_REPORT_DOCUMENT_ORDER) {
    int (*compare)(const void *, const void *) =
        filter->order == UMI_CTEST_REPORT_NAME_ORDER      ? Names
        : filter->order == UMI_CTEST_REPORT_LONGEST_FIRST ? Durations
                                                          : Outcomes;
    qsort(view->rows, view->summary.matches, sizeof *view->rows, compare);
  }
  *out = view;
  return UMI_STATUS_OK;
}
UmiStatus UmiCtestReportViewRead(const UmiCtestReportView *view, UmiCtestReportViewSummary *out) {
  if (view == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = view->summary;
  return UMI_STATUS_OK;
}
const UmiCtestReportEntry *UmiCtestReportViewAt(const UmiCtestReportView *view, size_t index) {
  return view != NULL && index < view->summary.matches ? view->rows[index].entry : NULL;
}
