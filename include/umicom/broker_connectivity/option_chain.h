/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/option_chain.h
 * PURPOSE: Retain bounded option definitions separately from resolved tradable contracts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_OPTION_CHAIN_H
#define UMICOM_BROKER_CONNECTIVITY_OPTION_CHAIN_H
#include "umicom/broker_connectivity/connection.h"
#include "umicom/broker_connectivity/order_recovery.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_IBKR_OPTION_CHAIN_LIMIT 32U
#define UMI_IBKR_OPTION_EXPIRY_LIMIT 128U
#define UMI_IBKR_OPTION_STRIKE_LIMIT 512U
    typedef struct UmiIbkrOptionChainQuery
    {
        uint32_t underlyingContractId;
        char underlyingSymbol[96], underlyingSecurityType[24], exchange[64];
    } UmiIbkrOptionChainQuery;
    typedef struct UmiIbkrOptionChain
    {
        uint32_t underlyingContractId;
        char exchange[64], tradingClass[64], multiplier[96];
        size_t expiryCount, strikeCount;
        char expirations[UMI_IBKR_OPTION_EXPIRY_LIMIT][9];
        UmiIbkrOrderNumber strikes[UMI_IBKR_OPTION_STRIKE_LIMIT];
    } UmiIbkrOptionChain;
    typedef struct UmiIbkrOptionChainSnapshot
    {
        uint32_t requestId;
        UmiIbkrOptionChainQuery query;
        size_t count;
        uint64_t requestedAtMilliseconds, completedAtMilliseconds;
        bool complete, failed, abandoned, stale;
        int providerCode;
        char message[256];
    } UmiIbkrOptionChainSnapshot;
    /* A chain lists candidate expiries and strikes, not a Cartesian set of valid
 * contracts. Resolve a chosen contract with the broker before requesting its
 * quotes or constructing an order. The broker does not provide chain cancel;
 * Abandon retires only the local request and ignores its late replies. */
    UmiStatus UmiIbkrOptionChainQueryValidate(const UmiIbkrOptionChainQuery *);
    UmiStatus UmiIbkrOptionChainRequest(UmiIbkrConnection *, const UmiIbkrOptionChainQuery *,
                                        uint64_t nowMilliseconds, uint32_t *outRequest);
    UmiStatus UmiIbkrOptionChainCopy(const UmiIbkrConnection *, uint32_t request, uint64_t nowMilliseconds,
                                     UmiIbkrOptionChainSnapshot *out);
    UmiStatus UmiIbkrOptionChainItemCopy(const UmiIbkrConnection *, uint32_t request, size_t index,
                                         UmiIbkrOptionChain *out);
    UmiStatus UmiIbkrOptionChainAbandon(UmiIbkrConnection *, uint32_t request);
#ifdef __cplusplus
}
#endif
#endif
