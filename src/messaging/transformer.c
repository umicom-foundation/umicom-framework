/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/transformer.c
 *
 * PURPOSE:
 *   Implement sequential owned-message transformation with cleanup between stages and failure-safe output ownership.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

UmiStatus umi_transformer_chain_create(size_t capacity,
                                       UmiTransformerChain **out_chain)
{
    UmiTransformerChain *chain;
    if (capacity == 0U || out_chain == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_chain = NULL;
    chain = (UmiTransformerChain *)calloc(1U, sizeof(*chain));
    if (chain == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    chain->entries = (UmiTransformEntry *)calloc(capacity,
                                                 sizeof(*chain->entries));
    if (chain->entries == NULL) {
        free(chain);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    chain->capacity = capacity;
    *out_chain = chain;
    return UMI_STATUS_OK;
}

void umi_transformer_chain_destroy(UmiTransformerChain *chain)
{
    if (chain == NULL) return;
    free(chain->entries);
    free(chain);
}

UmiStatus umi_transformer_chain_add(UmiTransformerChain *chain,
                                    UmiMessageTransform transform,
                                    void *user_data)
{
    if (chain == NULL || transform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (chain->count >= chain->capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    chain->entries[chain->count].transform = transform;
    chain->entries[chain->count].user_data = user_data;
    chain->count++;
    return UMI_STATUS_OK;
}

UmiStatus umi_transformer_chain_apply(const UmiTransformerChain *chain,
                                      const UmiMessageEnvelope *input,
                                      UmiOwnedMessage *output)
{
    UmiOwnedMessage current;
    UmiOwnedMessage next;
    UmiStatus status;
    size_t index;
    if (chain == NULL || input == NULL || output == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(output, 0, sizeof(*output));
    status = umi_message_copy(input, &current);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < chain->count; ++index) {
        (void)memset(&next, 0, sizeof(next));
        status = chain->entries[index].transform(&current.envelope,
                                                 &next,
                                                 chain->entries[index].user_data);
        umi_message_dispose(&current);
        if (status != UMI_STATUS_OK) {
            umi_message_dispose(&next);
            return status;
        }
        current = next;
    }
    *output = current;
    return UMI_STATUS_OK;
}

size_t umi_transformer_chain_count(const UmiTransformerChain *chain)
{
    return chain != NULL ? chain->count : 0U;
}
