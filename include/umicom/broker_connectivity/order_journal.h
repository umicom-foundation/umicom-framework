/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/order_journal.h
 *
 * PURPOSE:
 *   Retain provider order identifiers and monotonic broker order state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BROKER_CONNECTIVITY_ORDER_JOURNAL_H
#define UMICOM_BROKER_CONNECTIVITY_ORDER_JOURNAL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BROKER_ORDER_JOURNAL_CAPACITY 256U

typedef struct UmiBrokerOrderRecord {
    UmiOrderRequest request;
    char providerOrderId[64];
    UmiOrderStatus status;
    uint64_t providerSequence;
    uint64_t submittedMilliseconds;
    uint64_t updatedMilliseconds;
} UmiBrokerOrderRecord;

typedef struct UmiBrokerOrderJournal {
    UmiBrokerOrderRecord records[UMI_BROKER_ORDER_JOURNAL_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiBrokerOrderJournal;

/* Initialise before use. Operations validate retained identity and timestamps,
 * but do not treat a journal record as order submission permission. The request
 * may be a minimal identity placeholder; submitters must separately validate it.
 * Updates accept direct broker state observations, preserve terminal states,
 * reject older sequence/time and reject conflicting reuse of a sequence.
 * Exact replay leaves the revision unchanged. A correction after a terminal
 * state needs explicit reconciliation rather than reopening through update.
 * Failed mutation leaves the journal unchanged; find returns NULL for invalid
 * storage. Returned records are borrowed until the next owner mutation. */
void umi_broker_order_journal_init(UmiBrokerOrderJournal *journal);
UmiStatus umi_broker_order_journal_add(
    UmiBrokerOrderJournal *journal,
    const UmiBrokerOrderRecord *record);
UmiStatus umi_broker_order_journal_update(
    UmiBrokerOrderJournal *journal,
    const char *clientOrderId,
    UmiOrderStatus status,
    uint64_t providerSequence,
    uint64_t updatedMilliseconds);
const UmiBrokerOrderRecord *umi_broker_order_journal_find(
    const UmiBrokerOrderJournal *journal,
    const char *clientOrderId);

#ifdef __cplusplus
}
#endif
#endif
