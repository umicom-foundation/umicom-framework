/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/discovery_report.h
 * PURPOSE: Export discovery evidence with its request scope and completeness.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_DISCOVERY_REPORT_H
#define UMICOM_BROKER_CONNECTIVITY_DISCOVERY_REPORT_H
#include "umicom/broker_connectivity/scanner.h"
#include "umicom/broker_connectivity/option_chain.h"
#include "umicom/broker_connectivity/observation_export.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* Capture on the connection's owning event loop. The returned document owns all
 * bytes and may then be handed to a file worker. Errors set *outDocument to NULL.
 * Retained stale and incomplete captures remain exportable with explicit flags. */
    UmiStatus UmiIbkrScannerExportCsv(const UmiIbkrConnection *, uint32_t request, uint64_t nowMilliseconds,
                                      uint64_t maximumAgeMilliseconds, UmiCsvDocument **outDocument);
    /* Expiries and strikes are separate records. The report never invents tradable
 * expiry/strike pairs by multiplying the two sets. */
    UmiStatus UmiIbkrOptionChainExportCsv(const UmiIbkrConnection *, uint32_t request,
                                          uint64_t nowMilliseconds, UmiCsvDocument **outDocument);
#ifdef __cplusplus
}
#endif
#endif
