/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/option_contract.h
 * PURPOSE: Resolve one selected option candidate to a broker contract identity.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_OPTION_CONTRACT_H
#define UMICOM_BROKER_CONNECTIVITY_OPTION_CONTRACT_H
#include "umicom/broker_connectivity/option_chain.h"
#include "umicom/broker_connectivity/contract_details.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiIbkrOptionSelection
    {
        uint32_t chainRequest;
        size_t chainIndex, expiryIndex, strikeIndex;
        char right;                      /* 'C' for call or 'P' for put. */
        char currency[16], exchange[64]; /* Explicit routing choice, not a listing guess. */
    } UmiIbkrOptionSelection;
    /* Resolve just one choice from a completed chain. Results use the existing
 * ContractDetailsCopy owner and completion marker. Never treat zero results,
 * partial results or an ambiguous result as permission to quote or trade. */
    UmiStatus UmiIbkrOptionContractRequest(UmiIbkrConnection *, const UmiIbkrOptionSelection *,
                                           uint64_t nowMilliseconds, uint32_t *outRequest);
#ifdef __cplusplus
}
#endif
#endif
