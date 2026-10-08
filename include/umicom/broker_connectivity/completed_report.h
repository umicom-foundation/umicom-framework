/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/completed_report.h
 * PURPOSE: Create an owned completed-order report with explicit scope and freshness.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_COMPLETED_REPORT_H
#define UMICOM_BROKER_CONNECTIVITY_COMPLETED_REPORT_H
#include "umicom/broker_connectivity/completed_orders.h"
#include "umicom/base/csv_document.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* Call on the connection's owning thread without concurrent pumping. A completed
 * request must have been accepted, but partial, failed or old observations may
 * be exported with their state clearly attached to every row. The CSV owns its
 * bytes and survives connection destruction. No network or filesystem work runs
 * here. On failure *outDocument is NULL, never a partly constructed report.
 *
 * Provider quantities and times remain text. The shared CSV policy quotes text
 * and guards formula prefixes; import these columns as text to retain spelling.
 * Times labelled monotonic are process-relative, not UTC. The report contains
 * the completed-order capture's full scope, not the account selector's scope.
 * Use UmiIbkrObservationsWriteNew to save it on a worker without replacing files. */
    UmiStatus UmiIbkrCompletedOrdersExportCsv(const UmiIbkrConnection *connection,
                                              uint64_t capturedAtMilliseconds,
                                              uint64_t maximumAgeMilliseconds, UmiCsvDocument **outDocument);
#ifdef __cplusplus
}
#endif
#endif
