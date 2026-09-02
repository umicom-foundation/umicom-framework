/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/transformer.h
 *
 * PURPOSE:
 *   Declare ordered message transformations that create new owned envelopes while preserving correlation and causation metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_TRANSFORMER_H
#define UMICOM_MESSAGING_TRANSFORMER_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/messaging/message.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UmiStatus (*UmiMessageTransform)(const UmiMessageEnvelope *input,
                                         UmiOwnedMessage *output,
                                         void *user_data);
/**
 * Represent the transformer chain data shared with callers of this public contract.
 */
typedef struct UmiTransformerChain UmiTransformerChain;

/**
 * Initialise transformer chain from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_transformer_chain_create(size_t capacity,
                                       UmiTransformerChain **out_chain);
/**
 * Release or reset state held by transformer chain so the same storage can be reused
 * safely.
 */
void umi_transformer_chain_destroy(UmiTransformerChain *chain);
/**
 * Add transformer chain only after its inputs and available capacity have been checked.
 */
UmiStatus umi_transformer_chain_add(UmiTransformerChain *chain,
                                    UmiMessageTransform transform,
                                    void *user_data);
/**
 * Perform transformer chain through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_transformer_chain_apply(const UmiTransformerChain *chain,
                                      const UmiMessageEnvelope *input,
                                      UmiOwnedMessage *output);
/**
 * Return the number of records represented by transformer chain without changing their
 * state.
 */
size_t umi_transformer_chain_count(const UmiTransformerChain *chain);

#ifdef __cplusplus
}
#endif

#endif
