/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/filter.h
 *
 * PURPOSE:
 *   Declare composable Integration Fabric filters that accept or reject messages without mutating authoritative state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef struct UmiFilterChain UmiFilterChain;

UmiStatus umi_filter_chain_create(size_t capacity,
                                  UmiFilterChain **out_chain);
void umi_filter_chain_destroy(UmiFilterChain *chain);
UmiStatus umi_filter_chain_add(UmiFilterChain *chain,
                               UmiMessagePredicate predicate,
                               void *user_data);
int umi_filter_chain_accepts(const UmiFilterChain *chain,
                             const UmiMessageEnvelope *message);
size_t umi_filter_chain_count(const UmiFilterChain *chain);

#ifdef __cplusplus
}
#endif

#endif
