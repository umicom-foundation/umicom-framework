/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/range_report.c
 * PURPOSE: Serialize measured candles through the shared typed CSV writer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/range_report.h"

enum RangeColumn {
  RECORD,
  INSTRUMENT,
  INTERVAL,
  CAPTURE_TIME,
  FIRST_TIME,
  LAST_TIME,
  COUNT,
  OPEN,
  HIGH,
  LOW,
  CLOSE,
  VOLUME,
  CHANGE,
  PERCENT,
  ELAPSED,
  LARGEST_STEP,
  RISING,
  FALLING,
  UNCHANGED,
  EVIDENCE,
  COLUMNS
};
static void Metadata(UmiCsvCell *row, const char *instrument, uint32_t interval,
                     uint64_t captured) {
  /* Empty fields remain unavailable instead of becoming misleading zeroes.
   * Identity is repeated so an extracted candle row keeps its context. */
  for (size_t index = 0U; index < COLUMNS; ++index)
    row[index] = UmiCsvText("");
  row[INSTRUMENT] = UmiCsvText(instrument);
  row[INTERVAL] = UmiCsvUnsigned(interval);
  row[CAPTURE_TIME] = UmiCsvUnsigned(captured);
  row[EVIDENCE] =
      UmiCsvText("retained chart snapshot; provider volume units; not an execution report");
}
UmiStatus UmiChartRangeReportCsv(const UmiChartRangeCapture *capture, const char *instrument_id,
                                 uint32_t interval_ms, uint64_t capture_time_ms,
                                 UmiCsvDocument **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = NULL;
  if (instrument_id == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t length = 0U;
  while (length < UMI_CHART_NAME_CAPACITY && instrument_id[length] != '\0')
    ++length;
  if (length == 0U || length == UMI_CHART_NAME_CAPACITY)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiChartCandleRange range;
  UmiStatus status = UmiChartRangeCaptureRead(capture, &range);
  if (status != UMI_STATUS_OK)
    return status;
  UmiCsvDocument *document = NULL;
  status = UmiCsvDocumentCreate(UMI_CSV_MAX_BYTES, &document);
  const char *headers[COLUMNS] = {"record_kind",
                                  "instrument_id",
                                  "chart_interval_ms",
                                  "capture_monotonic_ms",
                                  "first_open_time_utc_ms",
                                  "last_open_time_utc_ms",
                                  "candle_count",
                                  "open",
                                  "high",
                                  "low",
                                  "close",
                                  "volume",
                                  "change",
                                  "percent_change",
                                  "opening_time_span_ms",
                                  "largest_opening_step_ms",
                                  "rising_candles",
                                  "falling_candles",
                                  "unchanged_candles",
                                  "evidence"};
  UmiCsvCell row[COLUMNS];
  for (size_t index = 0U; index < COLUMNS; ++index)
    row[index] = UmiCsvText(headers[index]);
  if (status == UMI_STATUS_OK)
    status = UmiCsvDocumentAppendRow(document, row, COLUMNS);
  Metadata(row, instrument_id, interval_ms, capture_time_ms);
  row[RECORD] = UmiCsvText("summary");
  row[FIRST_TIME] = UmiCsvSigned(range.first_time_ms);
  row[LAST_TIME] = UmiCsvSigned(range.last_time_ms);
  row[COUNT] = UmiCsvUnsigned(range.count);
  row[OPEN] = UmiCsvReal(range.open);
  row[HIGH] = UmiCsvReal(range.high);
  row[LOW] = UmiCsvReal(range.low);
  row[CLOSE] = UmiCsvReal(range.close);
  row[VOLUME] = UmiCsvReal(range.volume);
  row[CHANGE] = UmiCsvReal(range.change);
  if (range.has_percent_change)
    row[PERCENT] = UmiCsvReal(range.percent_change);
  row[ELAPSED] = UmiCsvUnsigned(range.elapsed_ms);
  row[LARGEST_STEP] = UmiCsvUnsigned(range.largest_step_ms);
  row[RISING] = UmiCsvUnsigned(range.rising);
  row[FALLING] = UmiCsvUnsigned(range.falling);
  row[UNCHANGED] = UmiCsvUnsigned(range.unchanged);
  if (status == UMI_STATUS_OK)
    status = UmiCsvDocumentAppendRow(document, row, COLUMNS);
  for (size_t index = 0U; status == UMI_STATUS_OK && index < range.count; ++index) {
    UmiChartCandle candle;
    status = UmiChartRangeCaptureCandleAt(capture, index, &candle);
    if (status != UMI_STATUS_OK)
      break;
    Metadata(row, instrument_id, interval_ms, capture_time_ms);
    row[RECORD] = UmiCsvText("candle");
    row[FIRST_TIME] = UmiCsvSigned(candle.time_ms);
    row[COUNT] = UmiCsvUnsigned(1U);
    row[OPEN] = UmiCsvReal(candle.open);
    row[HIGH] = UmiCsvReal(candle.high);
    row[LOW] = UmiCsvReal(candle.low);
    row[CLOSE] = UmiCsvReal(candle.close);
    row[VOLUME] = UmiCsvReal(candle.volume);
    status = UmiCsvDocumentAppendRow(document, row, COLUMNS);
  }
  /* No partial report escapes when a row fails UTF-8, numeric or capacity
   * validation. The completed document owns all text independently of capture. */
  if (status != UMI_STATUS_OK) {
    UmiCsvDocumentDestroy(document);
    return status;
  }
  *out = document;
  return UMI_STATUS_OK;
}
