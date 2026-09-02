/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/filter.h
 *
 * PURPOSE:
 *   Declare composable Integration Fabric filters that accept or reject messages without mutating authoritative state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_FILTER_H
#define UMICOM_MESSAGING_FILTER_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/messaging/message.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*UmiMessagePredicate)(const UmiMessageEnvelope *message,
                                   void *user_data);
/**
 * Represent the filter chain data shared with callers of this public contract.
 */
typedef struct UmiFilterChain UmiFilterChain;

/**
 * Initialise filter chain from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_filter_chain_create(size_t capacity,
                                  UmiFilterChain **out_chain);
/**
 * Release or reset state held by filter chain so the same storage can be reused safely.
 */
void umi_filter_chain_destroy(UmiFilterChain *chain);
/**
 * Add filter chain only after its inputs and available capacity have been checked.
 */
UmiStatus umi_filter_chain_add(UmiFilterChain *chain,
                               UmiMessagePredicate predicate,
                               void *user_data);
/**
 * Provide the filter chain accepts operation used by this module and its client
 * applications.
 */
int umi_filter_chain_accepts(const UmiFilterChain *chain,
                             const UmiMessageEnvelope *message);
/**
 * Return the number of records represented by filter chain without changing their state.
 */
size_t umi_filter_chain_count(const UmiFilterChain *chain);

#ifdef __cplusplus
}
#endif

#endif
