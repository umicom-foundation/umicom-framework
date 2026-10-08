/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/scanner_catalog_private.h
 * PURPOSE: Own catalogue text and its exceptional bounded receive envelope.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_IBKR_SCANNER_CATALOG_PRIVATE_H
#define UMICOM_IBKR_SCANNER_CATALOG_PRIVATE_H
#include "umicom/broker_connectivity/scanner_catalog.h"
typedef struct UmiIbkrScannerCatalogStore
{
    UmiIbkrScannerCatalogSnapshot snapshot;
    char *xml;
} UmiIbkrScannerCatalogStore;
UmiStatus UmiIbkrScannerCatalogFrame(UmiIbkrConnection *, const unsigned char *, size_t, uint64_t);
UmiStatus UmiIbkrScannerCatalogBeginFrame(UmiIbkrConnection *, size_t);
bool UmiIbkrScannerCatalogPending(const UmiIbkrConnection *, uint64_t);
void UmiIbkrScannerCatalogClosed(UmiIbkrConnection *);
void UmiIbkrScannerCatalogDestroy(UmiIbkrConnection *);
#endif
