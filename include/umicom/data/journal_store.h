/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/journal_store.h
 *
 * PURPOSE:
 *   Declare durable journal append and consumer-position tracking over the Data Server message store.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_JOURNAL_STORE_H
#define UMICOM_DATA_JOURNAL_STORE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/data/message_store.h"
#include "umicom/data/store.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiJournalStore {
    UmiStore store;
    UmiMessageStore *messages;
    char namespace_name[128];
} UmiJournalStore;

UmiStatus umi_journal_store_init(UmiJournalStore *journal,
                                 const UmiStore *store,
                                 const char *namespace_name);
void umi_journal_store_dispose(UmiJournalStore *journal);
UmiStatus umi_journal_store_append(UmiJournalStore *journal,
                                   const UmiMessageEnvelope *message,
                                   uint64_t *out_sequence);
UmiStatus umi_journal_store_read(const UmiJournalStore *journal,
                                 size_t index,
                                 UmiOwnedMessage *out_message);
UmiStatus umi_journal_store_set_consumer_position(UmiJournalStore *journal,
                                                  const char *consumer,
                                                  uint64_t sequence);
UmiStatus umi_journal_store_consumer_position(const UmiJournalStore *journal,
                                              const char *consumer,
                                              uint64_t *out_sequence);
size_t umi_journal_store_count(const UmiJournalStore *journal);

#ifdef __cplusplus
}
#endif

#endif
