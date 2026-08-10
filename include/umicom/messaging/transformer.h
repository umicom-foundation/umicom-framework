/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/transformer.h
 *
 * PURPOSE:
 *   Declare ordered message transformations that create new owned envelopes while preserving correlation and causation metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef struct UmiTransformerChain UmiTransformerChain;

UmiStatus umi_transformer_chain_create(size_t capacity,
                                       UmiTransformerChain **out_chain);
void umi_transformer_chain_destroy(UmiTransformerChain *chain);
UmiStatus umi_transformer_chain_add(UmiTransformerChain *chain,
                                    UmiMessageTransform transform,
                                    void *user_data);
UmiStatus umi_transformer_chain_apply(const UmiTransformerChain *chain,
                                      const UmiMessageEnvelope *input,
                                      UmiOwnedMessage *output);
size_t umi_transformer_chain_count(const UmiTransformerChain *chain);

#ifdef __cplusplus
}
#endif

#endif
