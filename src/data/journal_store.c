/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/journal_store.c
 *
 * PURPOSE:
 *   Implement durable journal append/read delegation and independent consumer cursor persistence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/journal_store.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

UmiStatus umi_journal_store_init(UmiJournalStore *journal,
                                 const UmiStore *store,
                                 const char *namespace_name)
{
    UmiStatus status;
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

void umi_journal_store_dispose(UmiJournalStore *journal)
{
    if (journal == NULL) return;
    umi_message_store_destroy(journal->messages);
    (void)memset(journal, 0, sizeof(*journal));
}

UmiStatus umi_journal_store_append(UmiJournalStore *journal,
                                   const UmiMessageEnvelope *message,
                                   uint64_t *out_sequence)
{
    return journal != NULL
        ? umi_message_store_append(journal->messages, message, out_sequence)
        : UMI_STATUS_INVALID_ARGUMENT;
}

UmiStatus umi_journal_store_read(const UmiJournalStore *journal,
                                 size_t index,
                                 UmiOwnedMessage *out_message)
{
    return journal != NULL
        ? umi_message_store_read(journal->messages, index, out_message)
        : UMI_STATUS_INVALID_ARGUMENT;
}

static UmiStatus position_key(const UmiJournalStore *journal,
                              const char *consumer,
                              char *out_key,
                              size_t capacity)
{
    int written;
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

UmiStatus umi_journal_store_set_consumer_position(UmiJournalStore *journal,
                                                  const char *consumer,
                                                  uint64_t sequence)
{
    char key[320];
    char value[32];
    UmiStatus status = position_key(journal, consumer, key, sizeof(key));
    if (status != UMI_STATUS_OK) return status;
    (void)snprintf(value, sizeof(value), "%llu",
                   (unsigned long long)sequence);
    return umi_store_set(&journal->store, key, value);
}

UmiStatus umi_journal_store_consumer_position(const UmiJournalStore *journal,
                                              const char *consumer,
                                              uint64_t *out_sequence)
{
    char key[320];
    char value[32];
    char *end = NULL;
    unsigned long long parsed;
    UmiStatus status;
    if (out_sequence == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = position_key(journal, consumer, key, sizeof(key));
    if (status != UMI_STATUS_OK) return status;
    status = umi_store_get(&journal->store, key, value, sizeof(value));
    if (status == UMI_STATUS_NOT_FOUND) {
        *out_sequence = 0U;
        return UMI_STATUS_OK;
    }
    if (status != UMI_STATUS_OK) return status;
    errno = 0;
    parsed = strtoull(value, &end, 10);
    if (errno != 0 || end == value || *end != '\0') {
        return UMI_STATUS_PARSE_ERROR;
    }
    *out_sequence = (uint64_t)parsed;
    return UMI_STATUS_OK;
}

size_t umi_journal_store_count(const UmiJournalStore *journal)
{
    return journal != NULL
        ? umi_message_store_count(journal->messages)
        : 0U;
}
