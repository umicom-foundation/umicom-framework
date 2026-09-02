/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/message_store.h
 *
 * PURPOSE:
 *   Declare durable message persistence, ordered reads and replay-source adaptation over the authoritative Data Server.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_MESSAGE_STORE_H
#define UMICOM_DATA_MESSAGE_STORE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/data/store.h"
#include "umicom/messaging/message.h"
#include "umicom/messaging/replay.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the message store data shared with callers of this public contract.
 */
typedef struct UmiMessageStore UmiMessageStore;

/**
 * Initialise message store from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_message_store_create(const UmiStore *store,
                                   const char *namespace_name,
                                   UmiMessageStore **out_message_store);
/**
 * Release or reset state held by message store so the same storage can be reused safely.
 */
void umi_message_store_destroy(UmiMessageStore *message_store);
/**
 * Add message store only after its inputs and available capacity have been checked.
 */
UmiStatus umi_message_store_append(UmiMessageStore *message_store,
                                   const UmiMessageEnvelope *message,
                                   uint64_t *out_sequence);
/**
 * Read message store into validated module state and return a status when input cannot be
 * used.
 */
UmiStatus umi_message_store_read(const UmiMessageStore *message_store,
                                 size_t index,
                                 UmiOwnedMessage *out_message);
/**
 * Return the number of records represented by message store without changing their state.
 */
size_t umi_message_store_count(const UmiMessageStore *message_store);
/**
 * Provide the message store replay source operation used by this module and its client
 * applications.
 */
UmiReplaySource umi_message_store_replay_source(UmiMessageStore *message_store);

#ifdef __cplusplus
}
#endif

#endif
