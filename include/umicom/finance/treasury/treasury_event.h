/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/treasury_event.h
 *
 * PURPOSE:
 *   Record sequence-ordered treasury domain events with event timestamp.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_TREASURY_EVENT_H
#define UMICOM_FINANCE_TREASURY_TREASURY_EVENT_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury treasury event data shared with callers of this public contract.
 */
typedef struct UmiTreasuryTreasuryEvent {
    char id[UMI_TREASURY_ID_CAPACITY];
    uint64_t sequence;
    int64_t event_epoch_millis;
} UmiTreasuryTreasuryEvent;
/**
 * Initialise treasury treasury event from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_treasury_event_init(UmiTreasuryTreasuryEvent *value,
    const char *id,
    uint64_t sequence,
    int64_t event_epoch_millis);
/**
 * Check that treasury treasury event satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_treasury_event_valid(const UmiTreasuryTreasuryEvent *value);
/**
 * Provide the treasury treasury event event sequence operation used by this module and its
 * client applications.
 */
uint64_t umi_treasury_treasury_event_event_sequence(const UmiTreasuryTreasuryEvent *value);
#ifdef __cplusplus
}
#endif
#endif
