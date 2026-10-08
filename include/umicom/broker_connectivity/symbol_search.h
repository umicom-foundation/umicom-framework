/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/symbol_search.h
 * PURPOSE: Find broker contract candidates without automatically subscribing or trading.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_SYMBOL_SEARCH_H
#define UMICOM_BROKER_CONNECTIVITY_SYMBOL_SEARCH_H
#include "umicom/broker_connectivity/connection.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_IBKR_SYMBOL_LIMIT 16U
#define UMI_IBKR_DERIVATIVE_TYPE_LIMIT 16U
    typedef struct UmiIbkrSymbolCandidate
    {
        uint32_t contractId;
        char symbol[64], securityType[16], primaryExchange[64], currency[16];
        char description[256], issuerId[64];
        size_t derivativeTypeCount;
        char derivativeTypes[UMI_IBKR_DERIVATIVE_TYPE_LIMIT][16];
    } UmiIbkrSymbolCandidate;
    typedef struct UmiIbkrSymbolSearchSnapshot
    {
        uint32_t requestId;
        char pattern[128], message[256];
        uint64_t requestedAtMilliseconds, receivedAtMilliseconds;
        size_t count;
        bool complete, failed, stale;
        int providerCode;
        UmiIbkrSymbolCandidate rows[UMI_IBKR_SYMBOL_LIMIT];
    } UmiIbkrSymbolSearchSnapshot;
    /* Search for a ticker prefix or company-name fragment. One outstanding request
 * is owned at a time; subsequent requests are paced at least one second apart.
 * New successful requests replace the prior capture. Copy it first if needed.
 * A result is a candidate, not confirmation of exchange route or permission.
 * Outputs and retained state are unchanged if the request cannot be queued. */
    UmiStatus UmiIbkrSymbolSearchRequest(UmiIbkrConnection *connection, const char *pattern,
                                         uint64_t nowMilliseconds, uint32_t *outRequestId);
    UmiStatus UmiIbkrSymbolSearchCopy(const UmiIbkrConnection *connection, uint32_t requestId,
                                      uint64_t nowMilliseconds, UmiIbkrSymbolSearchSnapshot *out);
    /* Abandon only the local search. This protocol has no search-cancel message;
 * late responses retain the retired identity and cannot complete a new search. */
    UmiStatus UmiIbkrSymbolSearchAbandon(UmiIbkrConnection *connection, uint32_t requestId);
#ifdef __cplusplus
}
#endif
#endif
