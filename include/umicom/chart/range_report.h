/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/range_report.h
 * PURPOSE: Export owned chart measurement evidence with its instrument and timeframe.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CHART_RANGE_REPORT_H
#define UMICOM_CHART_RANGE_REPORT_H
#include "umicom/base/csv_document.h"
#include "umicom/chart/range_capture.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Build an owned CSV containing one summary and the measured candle rows.
 * instrument_id must be nonempty terminated UTF-8 shorter than
 * UMI_CHART_NAME_CAPACITY. interval_ms == 0 identifies the source timeframe.
 * capture_time_ms is a caller-supplied monotonic receipt/capture time, not UTC.
 * Numeric values describe copied chart doubles, not broker exact-decimal text.
 * The report asserts no source completeness, account environment or tradability.
 * It repeats instrument/timeframe/capture identity on every row. The caller
 * destroys the returned document. On every failure *out is NULL. */
UmiStatus UmiChartRangeReportCsv(const UmiChartRangeCapture *capture, const char *instrument_id,
                                 uint32_t interval_ms, uint64_t capture_time_ms,
                                 UmiCsvDocument **out);
#ifdef __cplusplus
}
#endif
#endif
