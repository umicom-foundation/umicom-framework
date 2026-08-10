/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/inbox.h
 *
 * PURPOSE:
 *   Declare duplicate-delivery protection for consumers by recording processed message identifiers and processing outcomes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_INBOX_H
#define UMICOM_MESSAGING_INBOX_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiInbox UmiInbox;

typedef struct UmiInboxStats {
    size_t capacity;
    size_t processed;
    uint64_t duplicates;
} UmiInboxStats;

UmiStatus umi_inbox_create(size_t capacity, UmiInbox **out_inbox);
void umi_inbox_destroy(UmiInbox *inbox);
int umi_inbox_contains(const UmiInbox *inbox, uint64_t message_id);
UmiStatus umi_inbox_mark_processed(UmiInbox *inbox,
                                   uint64_t message_id,
                                   UmiStatus outcome);
UmiStatus umi_inbox_outcome(const UmiInbox *inbox,
                            uint64_t message_id,
                            UmiStatus *out_outcome);
UmiInboxStats umi_inbox_stats(const UmiInbox *inbox);

#ifdef __cplusplus
}
#endif

#endif
