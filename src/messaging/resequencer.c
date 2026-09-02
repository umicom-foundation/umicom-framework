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

/*
 * Initialise resequencer from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_resequencer_create(uint64_t first_sequence,
                                 size_t capacity,
                                 UmiResequencer **out_resequencer)
{
    UmiResequencer *resequencer;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (first_sequence == 0U || capacity == 0U || out_resequencer == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_resequencer = NULL;
    resequencer = (UmiResequencer *)calloc(1U, sizeof(*resequencer));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (resequencer == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    resequencer->messages = (UmiOwnedMessage *)calloc(capacity,
                                                       sizeof(*resequencer->messages));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (resequencer->messages == NULL) {
        free(resequencer);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    resequencer->capacity = capacity;
    resequencer->expected = first_sequence;
    *out_resequencer = resequencer;
    return UMI_STATUS_OK;
}

/* Release or reset state held by resequencer so the same storage can be reused safely. */
void umi_resequencer_destroy(UmiResequencer *resequencer)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (resequencer == NULL) return;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < resequencer->count; ++index) {
        umi_message_dispose(&resequencer->messages[index]);
    }
    free(resequencer->messages);
    free(resequencer);
}

/* Add resequencer only after its inputs and available capacity have been checked. */
UmiStatus umi_resequencer_add(UmiResequencer *resequencer,
                              const UmiMessageEnvelope *message)
{
    size_t position = 0U;
    UmiOwnedMessage copy;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (resequencer == NULL || message == NULL || message->sequence == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (message->sequence < resequencer->expected) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (position < resequencer->count &&
           resequencer->messages[position].envelope.sequence < message->sequence) {
        position++;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position < resequencer->count &&
        resequencer->messages[position].envelope.sequence == message->sequence) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (resequencer->count >= resequencer->capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = umi_message_copy(message, &copy);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/* Provide the resequencer next operation used by this module and its client applications. */
UmiStatus umi_resequencer_next(UmiResequencer *resequencer,
                               UmiOwnedMessage *out_message)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (resequencer == NULL || out_message == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (resequencer->count == 0U ||
        resequencer->messages[0].envelope.sequence != resequencer->expected) {
        return UMI_STATUS_NOT_FOUND;
    }
    *out_message = resequencer->messages[0];
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Provide the resequencer expected operation used by this module and its client
 * applications.
 */
uint64_t umi_resequencer_expected(const UmiResequencer *resequencer)
{
    return resequencer != NULL ? resequencer->expected : 0U;
}

/*
 * Provide the resequencer buffered operation used by this module and its client
 * applications.
 */
size_t umi_resequencer_buffered(const UmiResequencer *resequencer)
{
    return resequencer != NULL ? resequencer->count : 0U;
}
