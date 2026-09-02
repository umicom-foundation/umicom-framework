/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/registry.h
 *
 * PURPOSE:
 *   Expose the schema and channel registry used by all context publishers and subscribers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_REGISTRY_H
#define UMICOM_CONTEXT_CHANNEL_REGISTRY_H
#include "umicom/context_channel/schema.h"
#include "umicom/context_channel/channel.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context registry data shared with callers of this public contract.
 */
typedef struct UmiContextRegistry { UmiContextSchemaStore schemas; UmiContextChannelStore channels; uint64_t revision; } UmiContextRegistry;
/**
 * Initialise context registry from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_registry_init(UmiContextRegistry *registry);
/**
 * Provide the context registry register schema operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_registry_register_schema(UmiContextRegistry *registry,const UmiContextSchema *schema);
/**
 * Provide the context registry register channel operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_registry_register_channel(UmiContextRegistry *registry,const UmiContextChannel *channel);
/**
 * Provide the context registry schema operation used by this module and its client
 * applications.
 */
const UmiContextSchema *umi_context_registry_schema(const UmiContextRegistry *registry,const char *schema_id);
/**
 * Provide the context registry channel operation used by this module and its client
 * applications.
 */
const UmiContextChannel *umi_context_registry_channel(const UmiContextRegistry *registry,const char *channel_id);
/**
 * Provide the context registry validate payload operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_registry_validate_payload(const UmiContextRegistry *registry,const char *channel_id,const UmiContextPayload *payload);
#ifdef __cplusplus
}
#endif
#endif
