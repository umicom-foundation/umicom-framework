/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/resequencer.c
 *
 * PURPOSE:
 *   Implement sorted insertion, duplicate-sequence rejection and release of consecutive messages from the expected sequence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/resequencer.h"

#include <stdlib.h>
#include <string.h>

struct UmiResequencer {
    UmiOwnedMessage *messages;
    size_t capacity;
    size_t count;
    uint64_t expected;
};

UmiStatus umi_resequencer_create(uint64_t first_sequence,
                                 size_t capacity,
                                 UmiResequencer **out_resequencer)
{
    UmiResequencer *resequencer;
    if (first_sequence == 0U || capacity == 0U || out_resequencer == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_resequencer = NULL;
    resequencer = (UmiResequencer *)calloc(1U, sizeof(*resequencer));
    if (resequencer == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    resequencer->messages = (UmiOwnedMessage *)calloc(capacity,
                                                       sizeof(*resequencer->messages));
    if (resequencer->messages == NULL) {
        free(resequencer);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    resequencer->capacity = capacity;
    resequencer->expected = first_sequence;
    *out_resequencer = resequencer;
    return UMI_STATUS_OK;
}

void umi_resequencer_destroy(UmiResequencer *resequencer)
{
    size_t index;
    if (resequencer == NULL) return;
    for (index = 0U; index < resequencer->count; ++index) {
        umi_message_dispose(&resequencer->messages[index]);
    }
    free(resequencer->messages);
    free(resequencer);
}

UmiStatus umi_resequencer_add(UmiResequencer *resequencer,
                              const UmiMessageEnvelope *message)
{
    size_t position = 0U;
    UmiOwnedMessage copy;
    UmiStatus status;
    if (resequencer == NULL || message == NULL || message->sequence == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (message->sequence < resequencer->expected) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    while (position < resequencer->count &&
           resequencer->messages[position].envelope.sequence < message->sequence) {
        position++;
    }
    if (position < resequencer->count &&
        resequencer->messages[position].envelope.sequence == message->sequence) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (resequencer->count >= resequencer->capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = umi_message_copy(message, &copy);
    if (status != UMI_STATUS_OK) return status;
    if (position < resequencer->count) {
        (void)memmove(&resequencer->messages[position + 1U],
                      &resequencer->messages[position],
                      (resequencer->count - position) *
                          sizeof(resequencer->messages[0]));
    }
    resequencer->messages[position] = copy;
    resequencer->count++;
    return UMI_STATUS_OK;
}

UmiStatus umi_resequencer_next(UmiResequencer *resequencer,
                               UmiOwnedMessage *out_message)
{
    if (resequencer == NULL || out_message == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (resequencer->count == 0U ||
        resequencer->messages[0].envelope.sequence != resequencer->expected) {
        return UMI_STATUS_NOT_FOUND;
    }
    *out_message = resequencer->messages[0];
    if (resequencer->count > 1U) {
        (void)memmove(&resequencer->messages[0],
                      &resequencer->messages[1],
                      (resequencer->count - 1U) *
                          sizeof(resequencer->messages[0]));
    }
    resequencer->count--;
    (void)memset(&resequencer->messages[resequencer->count],
                 0,
                 sizeof(resequencer->messages[0]));
    resequencer->expected++;
    return UMI_STATUS_OK;
}

uint64_t umi_resequencer_expected(const UmiResequencer *resequencer)
{
    return resequencer != NULL ? resequencer->expected : 0U;
}

size_t umi_resequencer_buffered(const UmiResequencer *resequencer)
{
    return resequencer != NULL ? resequencer->count : 0U;
}
