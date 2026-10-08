/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/scanner_catalog.h
 * PURPOSE: Inspect the broker scanner catalogue without interpreting or executing XML.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_SCANNER_CATALOG_H
#define UMICOM_BROKER_CONNECTIVITY_SCANNER_CATALOG_H
#include "umicom/broker_connectivity/connection.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_IBKR_SCANNER_CATALOG_BYTES (4U * 1024U * 1024U)
    typedef struct UmiIbkrScannerCatalogSnapshot
    {
        size_t byteCount;
        uint64_t requestedAtMilliseconds, receivedAtMilliseconds;
        bool requested, complete, failed, stale;
        char message[256];
    } UmiIbkrScannerCatalogSnapshot;
    /* This response has no request ID. Request at most once on a connection, even
 * after a timeout, so a late reply cannot be mistaken for a newer catalogue.
 * XML is retained as plain UTF-8 text; no entities, scripts or network resources
 * are evaluated. Consumers may build a separately validated picker later.
 * Use these functions on the connection's event-loop thread. */
    UmiStatus UmiIbkrScannerCatalogRequest(UmiIbkrConnection *, uint64_t nowMilliseconds);
    UmiStatus UmiIbkrScannerCatalogCopy(const UmiIbkrConnection *, uint64_t nowMilliseconds,
                                        UmiIbkrScannerCatalogSnapshot *out);
    /* Read byteCount from Copy, allocate byteCount + 1, then copy the whole text.
 * Failure leaves the destination untouched. Retained completed text remains
 * inspectable after disconnect; consult the snapshot's stale flag. */
    UmiStatus UmiIbkrScannerCatalogTextCopy(const UmiIbkrConnection *, char *out, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
