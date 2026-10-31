/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/time_sales_report.h
 *
 * PURPOSE:
 *   Capture an owned, bounded Time and Sales window for native and headless clients.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_TIME_SALES_REPORT_H
#define UMICOM_BROKER_CONNECTIVITY_TIME_SALES_REPORT_H
#include "umicom/broker_connectivity/time_sales.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_IBKR_TIME_SALES_VIEW_LIMIT 64U
typedef struct UmiIbkrTimeSalesReportRow {
    UmiIbkrTimeSalesEvent event;
    bool stale; /* Receipt age; it does not prove exchange-time freshness. */
} UmiIbkrTimeSalesReportRow;
typedef struct UmiIbkrTimeSalesReport {
    UmiIbkrTimeSalesSnapshot capture;
    uint64_t observedAtMilliseconds;
    size_t count, omittedStoredEvents;
    UmiIbkrTimeSalesReportRow rows[UMI_IBKR_TIME_SALES_VIEW_LIMIT];
} UmiIbkrTimeSalesReport;

/* Owns the newest bounded window, in receipt order, at one observation time.
 * The caller must use the connection owner thread. Copies do not pump I/O or
 * deduplicate equal events. Allocate reports on the heap in native callbacks.
 * On any error the destination remains unchanged. Omitted stored events and
 * events already evicted by the stream are counted separately. */
UmiStatus UmiIbkrTimeSalesReportCopy(const UmiIbkrConnection *, uint32_t requestId,
    uint64_t nowMilliseconds, uint64_t maximumAgeMilliseconds, UmiIbkrTimeSalesReport *);
#ifdef __cplusplus
}
#endif
#endif
