/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/inbox.h
 *
 * PURPOSE:
 *   Declare duplicate-delivery protection for consumers by recording processed message identifiers and processing outcomes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_INBOX_H
#define UMICOM_MESSAGING_INBOX_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the inbox data shared with callers of this public contract.
 */
typedef struct UmiInbox UmiInbox;

/**
 * Represent the inbox stats data shared with callers of this public contract.
 */
typedef struct UmiInboxStats {
    size_t capacity;
    size_t processed;
    uint64_t duplicates;
} UmiInboxStats;

/**
 * Initialise inbox from caller-provided values so later operations receive a known state.
 */
UmiStatus umi_inbox_create(size_t capacity, UmiInbox **out_inbox);
/**
 * Release or reset state held by inbox so the same storage can be reused safely.
 */
void umi_inbox_destroy(UmiInbox *inbox);
/**
 * Provide the inbox contains operation used by this module and its client applications.
 */
int umi_inbox_contains(const UmiInbox *inbox, uint64_t message_id);
/**
 * Provide the inbox mark processed operation used by this module and its client
 * applications.
 */
UmiStatus umi_inbox_mark_processed(UmiInbox *inbox,
                                   uint64_t message_id,
                                   UmiStatus outcome);
/**
 * Provide the inbox outcome operation used by this module and its client applications.
 */
UmiStatus umi_inbox_outcome(const UmiInbox *inbox,
                            uint64_t message_id,
                            UmiStatus *out_outcome);
/**
 * Provide the inbox stats operation used by this module and its client applications.
 */
UmiInboxStats umi_inbox_stats(const UmiInbox *inbox);

#ifdef __cplusplus
}
#endif

#endif
