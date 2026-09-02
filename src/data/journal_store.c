/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/journal_store.c
 *
 * PURPOSE:
 *   Implement durable journal append/read delegation and independent consumer cursor persistence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/journal_store.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Initialise journal store from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_journal_store_init(UmiJournalStore *journal,
                                 const UmiStore *store,
                                 const char *namespace_name)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (journal == NULL || store == NULL || namespace_name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(journal, 0, sizeof(*journal));
    journal->store = *store;
    (void)snprintf(journal->namespace_name,
                   sizeof(journal->namespace_name),
                   "%s",
                   namespace_name);
    status = umi_message_store_create(store,
                                      namespace_name,
                                      &journal->messages);
    return status;
}

/* Release or reset state held by journal store so the same storage can be reused safely. */
void umi_journal_store_dispose(UmiJournalStore *journal)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (journal == NULL) return;
    umi_message_store_destroy(journal->messages);
    (void)memset(journal, 0, sizeof(*journal));
}

/* Add journal store only after its inputs and available capacity have been checked. */
UmiStatus umi_journal_store_append(UmiJournalStore *journal,
                                   const UmiMessageEnvelope *message,
                                   uint64_t *out_sequence)
{
    return journal != NULL
        ? umi_message_store_append(journal->messages, message, out_sequence)
        : UMI_STATUS_INVALID_ARGUMENT;
}

/*
 * Read journal store into validated module state and return a status when input cannot be
 * used.
 */
UmiStatus umi_journal_store_read(const UmiJournalStore *journal,
                                 size_t index,
                                 UmiOwnedMessage *out_message)
{
    return journal != NULL
        ? umi_message_store_read(journal->messages, index, out_message)
        : UMI_STATUS_INVALID_ARGUMENT;
}

/* Provide the position key operation used by this module and its client applications. */
static UmiStatus position_key(const UmiJournalStore *journal,
                              const char *consumer,
                              char *out_key,
                              size_t capacity)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (journal == NULL || consumer == NULL || out_key == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(out_key,
                       capacity,
                       "journal.%s.consumer.%s",
                       journal->namespace_name,
                       consumer);
    return written < 0 || (size_t)written >= capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}

/*
 * Provide the journal store set consumer position operation used by this module and its
 * client applications.
 */
UmiStatus umi_journal_store_set_consumer_position(UmiJournalStore *journal,
                                                  const char *consumer,
                                                  uint64_t sequence)
{
    char key[320];
    char value[32];
    UmiStatus status = position_key(journal, consumer, key, sizeof(key));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)snprintf(value, sizeof(value), "%llu",
                   (unsigned long long)sequence);
    return umi_store_set(&journal->store, key, value);
}

/*
 * Provide the journal store consumer position operation used by this module and its client
 * applications.
 */
UmiStatus umi_journal_store_consumer_position(const UmiJournalStore *journal,
                                              const char *consumer,
                                              uint64_t *out_sequence)
{
    char key[320];
    char value[32];
    char *end = NULL;
    unsigned long long parsed;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_sequence == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = position_key(journal, consumer, key, sizeof(key));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_store_get(&journal->store, key, value, sizeof(value));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_NOT_FOUND) {
        *out_sequence = 0U;
        return UMI_STATUS_OK;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    errno = 0;
    parsed = strtoull(value, &end, 10);
    /* Apply this branch only when its contract condition is satisfied. */
    if (errno != 0 || end == value || *end != '\0') {
        return UMI_STATUS_PARSE_ERROR;
    }
    *out_sequence = (uint64_t)parsed;
    return UMI_STATUS_OK;
}

/* Return the number of records represented by journal store without changing their state. */
size_t umi_journal_store_count(const UmiJournalStore *journal)
{
    return journal != NULL
        ? umi_message_store_count(journal->messages)
        : 0U;
}
