/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/ctest_report_view_private.h
 *
 * PURPOSE:
 *   Keep report selection indices separate from borrowed immutable result records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CTEST_REPORT_VIEW_PRIVATE_H
#define UMICOM_CTEST_REPORT_VIEW_PRIVATE_H
#include "umicom/testing/ctest_report_view.h"
typedef struct ReportViewRow {
  const UmiCtestReportEntry *entry;
  size_t source_index;
} ReportViewRow;
struct UmiCtestReportView {
  UmiCtestReportViewSummary summary;
  ReportViewRow *rows;
};
#endif
