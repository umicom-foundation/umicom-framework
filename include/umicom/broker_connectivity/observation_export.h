/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/observation_export.h
 * PURPOSE: Export selected-account observations with explicit completeness and receipt times.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_OBSERVATION_EXPORT_H
#define UMICOM_BROKER_CONNECTIVITY_OBSERVATION_EXPORT_H
#include "umicom/broker_connectivity/connection.h"
#include "umicom/base/csv_document.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* Snapshot access must be serialized with its connection owner. Capture only
 * after a selected-account request exists; partial and disconnected observations
 * may be exported, with their original completion flags and stale state. The
 * selected account must belong to this snapshot's authorised account list.
 * Receipt/capture times are process-monotonic milliseconds, not UTC timestamps.
 * A metadata row is always present, even when no value or position arrived.
 * Provider decimals remain text: no rounding, FX conversion or aggregation.
 * The CSV text policy escapes formula prefixes; import provider columns as text
 * when a spreadsheet must retain identifiers, unset values and decimal spelling.
 * The result owns all bytes and survives source changes. On failure *out=NULL;
 * no partial document is exposed. No connection, request, order or storage I/O. */
    UmiStatus UmiIbkrObservationsExportCsv(const UmiIbkrConnectionSnapshot *snapshot,
                                           uint64_t capturedAtMilliseconds, UmiCsvDocument **out);
    /* Write an owned report to a new ordinary absolute path, on a worker. Existing
 * files are refused. A cancelled or failed write may leave a partial new file;
 * it is retained for inspection, never deleted. Keep the document/token alive
 * and unchanged until return. NULL cancellation is allowed. */
    UmiStatus UmiIbkrObservationsWriteNew(const UmiCsvDocument *document, const char *absolutePath,
                                          const UmiCancellationToken *cancel);
#ifdef __cplusplus
}
#endif
#endif
