/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/realtime_report.h
 * PURPOSE: Export frozen streaming observations with their freshness and retention scope.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_REALTIME_REPORT_H
#define UMICOM_BROKER_CONNECTIVITY_REALTIME_REPORT_H
#include "umicom/broker_connectivity/realtime_bars.h"
#include "umicom/broker_connectivity/historical_report.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* Export owns its bytes and never borrows the connection after returning.
 * Stale, failed or cancelled captures remain exportable with explicit flags.
 * No output path is opened here; use the existing new-file observation writer. */
    UmiStatus UmiIbkrRealtimeExportCsv(const UmiIbkrConnection *, uint32_t requestId,
                                       uint64_t nowMilliseconds, uint64_t maximumAgeMilliseconds,
                                       UmiCsvDocument **outDocument);
#ifdef __cplusplus
}
#endif
#endif
