/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/discovery_private.h
 * PURPOSE: Keep scanner and chain storage private to the existing connection owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_IBKR_DISCOVERY_PRIVATE_H
#define UMICOM_IBKR_DISCOVERY_PRIVATE_H
#include "umicom/broker_connectivity/scanner.h"
#include "umicom/broker_connectivity/contract_details.h"
#include "umicom/broker_connectivity/option_chain.h"
typedef struct UmiIbkrScannerStore
{
    UmiIbkrScannerSnapshot snapshot;
    UmiIbkrScannerRow rows[UMI_IBKR_SCANNER_ROW_LIMIT];
} UmiIbkrScannerStore;
typedef struct UmiIbkrOptionChainStore
{
    UmiIbkrOptionChainSnapshot snapshot;
    UmiIbkrOptionChain *items[UMI_IBKR_OPTION_CHAIN_LIMIT];
} UmiIbkrOptionChainStore;
UmiStatus UmiIbkrScannerFilters(const UmiIbkrScannerQuery *, char out[512]);
bool UmiIbkrDiscoveryDate(const char *);
UmiIbkrScannerStore *UmiIbkrScannerFind(const UmiIbkrConnection *, uint32_t);
UmiStatus UmiIbkrScannerFrame(UmiIbkrConnection *, const unsigned char *, size_t, uint64_t);
UmiStatus UmiIbkrOptionChainFrame(UmiIbkrConnection *, uint64_t, const unsigned char *, size_t, uint64_t);
bool UmiIbkrScannerProviderMessage(UmiIbkrConnection *, const char *, int, const char *);
bool UmiIbkrOptionChainProviderMessage(UmiIbkrConnection *, const char *, int, const char *);
void UmiIbkrDiscoveryClosed(UmiIbkrConnection *);
void UmiIbkrDiscoveryDestroy(UmiIbkrConnection *);
void UmiIbkrOptionChainStoreFree(UmiIbkrOptionChainStore *);
bool UmiIbkrContractIdentityMatches(const UmiIbkrContractDetailsSnapshot *,
                                    const UmiIbkrContractDescription *);
bool UmiIbkrOptionChainPending(const UmiIbkrConnection *, uint64_t);
#endif
