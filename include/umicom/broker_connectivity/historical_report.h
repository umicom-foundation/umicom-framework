/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/historical_report.h
 * PURPOSE: Export historical bars with their request, availability and capture state attached.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_HISTORICAL_REPORT_H
#define UMICOM_BROKER_CONNECTIVITY_HISTORICAL_REPORT_H
#include "umicom/broker_connectivity/historical_bars.h"
#include "umicom/base/csv_document.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* Call on the connection's owning thread without concurrent pumping. A capture
 * must exist, but failed, pending, cancelled and old captures can be documented.
 * Every row carries its query and state. The returned CSV owns its bytes and
 * can be written later through UmiIbkrObservationsWriteNew after disconnect.
 * Provider numeric text uses the shared formula-prefix protection; import it
 * as text. Monotonic times are process-relative; bar times are Unix UTC millis.
 * Failure sets *outDocument to NULL. No network or filesystem I/O runs here. */
    UmiStatus UmiIbkrHistoricalExportCsv(const UmiIbkrConnection *connection, uint32_t requestId,
                                         uint64_t capturedAtMilliseconds, uint64_t maximumAgeMilliseconds,
                                         UmiCsvDocument **outDocument);
#ifdef __cplusplus
}
#endif
#endif
