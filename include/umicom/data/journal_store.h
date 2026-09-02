/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/journal_store.h
 *
 * PURPOSE:
 *   Declare durable journal append and consumer-position tracking over the Data Server message store.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the journal store data shared with callers of this public contract.
 */
typedef struct UmiJournalStore {
    UmiStore store;
    UmiMessageStore *messages;
    char namespace_name[128];
} UmiJournalStore;

/**
 * Initialise journal store from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_journal_store_init(UmiJournalStore *journal,
                                 const UmiStore *store,
                                 const char *namespace_name);
/**
 * Release or reset state held by journal store so the same storage can be reused safely.
 */
void umi_journal_store_dispose(UmiJournalStore *journal);
/**
 * Add journal store only after its inputs and available capacity have been checked.
 */
UmiStatus umi_journal_store_append(UmiJournalStore *journal,
                                   const UmiMessageEnvelope *message,
                                   uint64_t *out_sequence);
/**
 * Read journal store into validated module state and return a status when input cannot be
 * used.
 */
UmiStatus umi_journal_store_read(const UmiJournalStore *journal,
                                 size_t index,
                                 UmiOwnedMessage *out_message);
/**
 * Provide the journal store set consumer position operation used by this module and its
 * client applications.
 */
UmiStatus umi_journal_store_set_consumer_position(UmiJournalStore *journal,
                                                  const char *consumer,
                                                  uint64_t sequence);
/**
 * Provide the journal store consumer position operation used by this module and its client
 * applications.
 */
UmiStatus umi_journal_store_consumer_position(const UmiJournalStore *journal,
                                              const char *consumer,
                                              uint64_t *out_sequence);
/**
 * Return the number of records represented by journal store without changing their state.
 */
size_t umi_journal_store_count(const UmiJournalStore *journal);

#ifdef __cplusplus
}
#endif

#endif
