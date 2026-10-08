/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/contract_details.h
 * PURPOSE: Inspect instrument metadata without inferring execution permission.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_CONTRACT_DETAILS_H
#define UMICOM_BROKER_CONNECTIVITY_CONTRACT_DETAILS_H
#include "umicom/broker_connectivity/quotes.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_IBKR_CONTRACT_DETAILS_LIMIT 8U
    typedef struct UmiIbkrContractDescription
    {
        uint32_t contractId;
        char symbol[96], securityType[24], expiry[96], strike[96], right[16];
        char exchange[64], primaryExchange[64], currency[16], localSymbol[96], tradingClass[64];
        char marketName[128], longName[512], multiplier[96], minimumTick[96], priceMagnifier[32];
        char orderTypes[2048], validExchanges[2048], marketRuleIds[2048];
        char minimumSize[96], sizeIncrement[96], suggestedSizeIncrement[96];
    } UmiIbkrContractDescription;
    /* A descriptive lookup records every field needed to verify the returned
     * option. This is separate from the established exact-conId request. */
    typedef struct UmiIbkrOptionIdentity {
        char symbol[96], securityType[24], expiry[9], strike[96], right[2];
        char multiplier[96], exchange[64], currency[16], tradingClass[64];
    } UmiIbkrOptionIdentity;
    typedef struct UmiIbkrContractDetailsSnapshot
    {
        uint32_t requestId;
        UmiIbkrQuoteContract requestedContract;
        uint64_t requestedAtMilliseconds, completedAtMilliseconds;
        size_t count;
        bool complete, failed, stale;
        int providerCode;
        char message[512];
        UmiIbkrContractDescription items[UMI_IBKR_CONTRACT_DETAILS_LIMIT];
        bool optionLookup;
        UmiIbkrOptionIdentity requestedOption;
    } UmiIbkrContractDetailsSnapshot;
    /* Queue one read-only exact-ID lookup on a READY connection using protocol
 * size-rule metadata (164..176). A pending lookup is BUSY until completion,
 * failure, timeout or local abandonment. Quotes and contract lookups share
 * increasing request IDs. Failure preserves output and the previous lookup. */
    UmiStatus UmiIbkrContractDetailsRequest(UmiIbkrConnection *connection,
                                            const UmiIbkrQuoteContract *contract, uint64_t nowMilliseconds,
                                            uint32_t *outRequest);
    /* Copy historical provider text. Completion requires contractDetailsEnd;
 * partial metadata remains incomplete. Nothing is truncated. Unicode escapes
 * in longName remain literal. Lists do not prove restrictions or entitlement
 * for a contract/route combination, including FOK/AON. No money is booked.
 * Timeout and disconnection mark the returned copy stale. */
    UmiStatus UmiIbkrContractDetailsCopy(const UmiIbkrConnection *connection, uint32_t request,
                                         uint64_t nowMilliseconds, UmiIbkrContractDetailsSnapshot *out);
    /* Retire the local lookup. This negotiated protocol has no contract-query
 * cancellation command. Late replies are ignored; no order is cancelled. */
    UmiStatus UmiIbkrContractDetailsAbandon(UmiIbkrConnection *connection, uint32_t request);
#ifdef __cplusplus
}
#endif
#endif
