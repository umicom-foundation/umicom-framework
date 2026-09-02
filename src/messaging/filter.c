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

/*
 * Initialise filter chain from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_filter_chain_create(size_t capacity,
                                  UmiFilterChain **out_chain)
{
    UmiFilterChain *chain;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (capacity == 0U || out_chain == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_chain = NULL;
    chain = (UmiFilterChain *)calloc(1U, sizeof(*chain));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (chain == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    chain->entries = (UmiFilterEntry *)calloc(capacity,
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

/* Release or reset state held by filter chain so the same storage can be reused safely. */
void umi_filter_chain_destroy(UmiFilterChain *chain)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (chain == NULL) return;
    free(chain->entries);
    free(chain);
}

/* Add filter chain only after its inputs and available capacity have been checked. */
UmiStatus umi_filter_chain_add(UmiFilterChain *chain,
                               UmiMessagePredicate predicate,
                               void *user_data)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (chain == NULL || predicate == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (chain->count >= chain->capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    chain->entries[chain->count].predicate = predicate;
    chain->entries[chain->count].user_data = user_data;
    chain->count++;
    return UMI_STATUS_OK;
}

/*
 * Provide the filter chain accepts operation used by this module and its client
 * applications.
 */
int umi_filter_chain_accepts(const UmiFilterChain *chain,
                             const UmiMessageEnvelope *message)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (chain == NULL || message == NULL) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < chain->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!chain->entries[index].predicate(message,
                                             chain->entries[index].user_data)) {
            return 0;
        }
    }
    return 1;
}

/* Return the number of records represented by filter chain without changing their state. */
size_t umi_filter_chain_count(const UmiFilterChain *chain)
{
    return chain != NULL ? chain->count : 0U;
}
