/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/margin_call.h
 *
 * PURPOSE:
 *   Represent a margin call amount, agreed amount and lifecycle state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_MARGIN_CALL_H
#define UMICOM_FINANCE_TREASURY_MARGIN_CALL_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury margin call data shared with callers of this public contract.
 */
typedef struct UmiTreasuryMarginCall {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t called_minor;
    int64_t agreed_minor;
    UmiTreasuryMarginState state;
} UmiTreasuryMarginCall;
/**
 * Initialise treasury margin call from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_treasury_margin_call_init(UmiTreasuryMarginCall *value,
    const char *id,
    int64_t called_minor,
    int64_t agreed_minor,
    UmiTreasuryMarginState state);
/**
 * Check that treasury margin call satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_margin_call_valid(const UmiTreasuryMarginCall *value);
/**
 * Provide the treasury margin call unagreed minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_margin_call_unagreed_minor(const UmiTreasuryMarginCall *value);
#ifdef __cplusplus
}
#endif
#endif
