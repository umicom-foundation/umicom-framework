/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/ctest_report_view.h
 *
 * PURPOSE:
 *   Filter and page a borrowed immutable CTest report without changing its observations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_CTEST_REPORT_VIEW_H
#define UMICOM_TESTING_CTEST_REPORT_VIEW_H
#include "umicom/testing/ctest_report.h"
#ifdef __cplusplus
extern "C" {
#endif
enum {
  UMI_CTEST_VIEW_PASSED = 1U,
  UMI_CTEST_VIEW_FAILED = 2U,
  UMI_CTEST_VIEW_TIMED_OUT = 4U,
  UMI_CTEST_VIEW_SKIPPED = 8U,
  UMI_CTEST_VIEW_DISABLED = 16U,
  UMI_CTEST_VIEW_NOT_RUN = 32U,
  UMI_CTEST_VIEW_ALL = 63U
};
typedef enum UmiCtestReportOrder {
  UMI_CTEST_REPORT_DOCUMENT_ORDER = 0,
  UMI_CTEST_REPORT_NAME_ORDER = 1,
  UMI_CTEST_REPORT_LONGEST_FIRST = 2,
  UMI_CTEST_REPORT_OUTCOME_ORDER = 3
} UmiCtestReportOrder;
typedef struct UmiCtestReportFilter {
  unsigned outcomes;
  UmiCtestReportOrder order;
  char name_contains[256];
} UmiCtestReportFilter;
typedef struct UmiCtestReportViewSummary {
  UmiCtestReportSummary source;
  size_t matches;
} UmiCtestReportViewSummary;
typedef struct UmiCtestReportView UmiCtestReportView;
/* Default: all non-passed outcomes, document order, no name restriction. */
void UmiCtestReportFilterInit(UmiCtestReportFilter *filter);
/* Validate a complete filter using the imported report scalar grammar. */
UmiStatus UmiCtestReportFilterValidate(const UmiCtestReportFilter *filter);
/* The view BORROWS its immutable report. Destroy every view before destroying
 * that report. The filter is copied during creation and may then be reused.
 * Name filtering folds ASCII letters only and never evaluates an expression.
 * Zero outcome bits is valid and produces an empty selection. out becomes NULL
 * on failure. Owner-thread use; this service performs no I/O or execution. */
UmiStatus UmiCtestReportViewCreate(const UmiCtestReport *report, const UmiCtestReportFilter *filter,
                                   UmiCtestReportView **out);
void UmiCtestReportViewDestroy(UmiCtestReportView *view);
UmiStatus UmiCtestReportViewRead(const UmiCtestReportView *view, UmiCtestReportViewSummary *out);
const UmiCtestReportEntry *UmiCtestReportViewAt(const UmiCtestReportView *view, size_t index);
unsigned UmiCtestReportEntryOutcome(const UmiCtestReportEntry *entry);
const char *UmiCtestReportOutcomeText(unsigned outcome);
/* Plain escaped text, at most 128 rows per page. Query length with NULL,0;
 * required excludes the terminator. A short output remains unchanged.
 * Empty selections accept offset 0. Invalid offsets return NOT_FOUND.
 * Source data and output storage must not overlap. */
UmiStatus UmiCtestReportViewFormat(const UmiCtestReportView *view, size_t offset, size_t limit,
                                   char *out, size_t capacity, size_t *required);
#ifdef __cplusplus
}
#endif
#endif
