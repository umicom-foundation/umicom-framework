/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/transformer.c
 *
 * PURPOSE:
 *   Implement sequential owned-message transformation with cleanup between stages and failure-safe output ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/transformer.h"

#include <stdlib.h>
#include <string.h>

typedef struct UmiTransformEntry {
    UmiMessageTransform transform;
    void *user_data;
} UmiTransformEntry;

struct UmiTransformerChain {
    UmiTransformEntry *entries;
    size_t capacity;
    size_t count;
};

/*
 * Initialise transformer chain from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_transformer_chain_create(size_t capacity,
                                       UmiTransformerChain **out_chain)
{
    UmiTransformerChain *chain;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (capacity == 0U || out_chain == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_chain = NULL;
    chain = (UmiTransformerChain *)calloc(1U, sizeof(*chain));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (chain == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    chain->entries = (UmiTransformEntry *)calloc(capacity,
                                                 sizeof(*chain->entries));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (chain->entries == NULL) {
        free(chain);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    chain->capacity = capacity;
    *out_chain = chain;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by transformer chain so the same storage can be reused
 * safely.
 */
void umi_transformer_chain_destroy(UmiTransformerChain *chain)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (chain == NULL) return;
    free(chain->entries);
    free(chain);
}

/* Add transformer chain only after its inputs and available capacity have been checked. */
UmiStatus umi_transformer_chain_add(UmiTransformerChain *chain,
                                    UmiMessageTransform transform,
                                    void *user_data)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (chain == NULL || transform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (chain->count >= chain->capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    chain->entries[chain->count].transform = transform;
    chain->entries[chain->count].user_data = user_data;
    chain->count++;
    return UMI_STATUS_OK;
}

/*
 * Perform transformer chain through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_transformer_chain_apply(const UmiTransformerChain *chain,
                                      const UmiMessageEnvelope *input,
                                      UmiOwnedMessage *output)
{
    UmiOwnedMessage current;
    UmiOwnedMessage next;
    UmiStatus status;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (chain == NULL || input == NULL || output == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(output, 0, sizeof(*output));
    status = umi_message_copy(input, &current);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < chain->count; ++index) {
        (void)memset(&next, 0, sizeof(next));
        status = chain->entries[index].transform(&current.envelope,
                                                 &next,
                                                 chain->entries[index].user_data);
        umi_message_dispose(&current);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            umi_message_dispose(&next);
            return status;
        }
        current = next;
    }
    *output = current;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by transformer chain without changing their
 * state.
 */
size_t umi_transformer_chain_count(const UmiTransformerChain *chain)
{
    return chain != NULL ? chain->count : 0U;
}
