/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/filter.c
 *
 * PURPOSE:
 *   Implement ordered filter predicates with short-circuit rejection and bounded registration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/filter.h"

#include <stdlib.h>

typedef struct UmiFilterEntry {
    UmiMessagePredicate predicate;
    void *user_data;
} UmiFilterEntry;

struct UmiFilterChain {
    UmiFilterEntry *entries;
    size_t capacity;
    size_t count;
};

UmiStatus umi_filter_chain_create(size_t capacity,
                                  UmiFilterChain **out_chain)
{
    UmiFilterChain *chain;
    if (capacity == 0U || out_chain == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_chain = NULL;
    chain = (UmiFilterChain *)calloc(1U, sizeof(*chain));
    if (chain == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    chain->entries = (UmiFilterEntry *)calloc(capacity,
                                              sizeof(*chain->entries));
    if (chain->entries == NULL) {
        free(chain);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    chain->capacity = capacity;
    *out_chain = chain;
    return UMI_STATUS_OK;
}

void umi_filter_chain_destroy(UmiFilterChain *chain)
{
    if (chain == NULL) return;
    free(chain->entries);
    free(chain);
}

UmiStatus umi_filter_chain_add(UmiFilterChain *chain,
                               UmiMessagePredicate predicate,
                               void *user_data)
{
    if (chain == NULL || predicate == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (chain->count >= chain->capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    chain->entries[chain->count].predicate = predicate;
    chain->entries[chain->count].user_data = user_data;
    chain->count++;
    return UMI_STATUS_OK;
}

int umi_filter_chain_accepts(const UmiFilterChain *chain,
                             const UmiMessageEnvelope *message)
{
    size_t index;
    if (chain == NULL || message == NULL) return 0;
    for (index = 0U; index < chain->count; ++index) {
        if (!chain->entries[index].predicate(message,
                                             chain->entries[index].user_data)) {
            return 0;
        }
    }
    return 1;
}

size_t umi_filter_chain_count(const UmiFilterChain *chain)
{
    return chain != NULL ? chain->count : 0U;
}
