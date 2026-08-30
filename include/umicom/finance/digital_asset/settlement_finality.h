/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/settlement_finality.h
 *
 * PURPOSE:
 *   Evaluate provisional and final settlement against explicit confirmation policy.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_SETTLEMENT_FINALITY_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_SETTLEMENT_FINALITY_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/finance/digital_asset/confirmation_policy.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDigitalSettlementFinality {
    uint32_t confirmations;
    bool provisionally_settled;
    bool final_settled;
} UmiDigitalSettlementFinality;

/* Evaluate settlement state from current confirmations and network policy. */
UmiStatus umi_digital_asset_settlement_finality_evaluate(const UmiDigitalConfirmationPolicy *policy, uint32_t confirmations, UmiDigitalSettlementFinality *out_finality);

#ifdef __cplusplus
}
#endif

#endif
