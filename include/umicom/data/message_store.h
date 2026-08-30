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

typedef struct UmiMessageStore UmiMessageStore;

UmiStatus umi_message_store_create(const UmiStore *store,
                                   const char *namespace_name,
                                   UmiMessageStore **out_message_store);
void umi_message_store_destroy(UmiMessageStore *message_store);
UmiStatus umi_message_store_append(UmiMessageStore *message_store,
                                   const UmiMessageEnvelope *message,
                                   uint64_t *out_sequence);
UmiStatus umi_message_store_read(const UmiMessageStore *message_store,
                                 size_t index,
                                 UmiOwnedMessage *out_message);
size_t umi_message_store_count(const UmiMessageStore *message_store);
UmiReplaySource umi_message_store_replay_source(UmiMessageStore *message_store);

#ifdef __cplusplus
}
#endif

#endif
