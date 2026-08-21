/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/registry.h
 *
 * PURPOSE:
 *   Expose the schema and channel registry used by all context publishers and subscribers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_REGISTRY_H
#define UMICOM_CONTEXT_CHANNEL_REGISTRY_H
#include "umicom/context_channel/schema.h"
#include "umicom/context_channel/channel.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiContextRegistry { UmiContextSchemaStore schemas; UmiContextChannelStore channels; uint64_t revision; } UmiContextRegistry;
void umi_context_registry_init(UmiContextRegistry *registry);
UmiStatus umi_context_registry_register_schema(UmiContextRegistry *registry,const UmiContextSchema *schema);
UmiStatus umi_context_registry_register_channel(UmiContextRegistry *registry,const UmiContextChannel *channel);
const UmiContextSchema *umi_context_registry_schema(const UmiContextRegistry *registry,const char *schema_id);
const UmiContextChannel *umi_context_registry_channel(const UmiContextRegistry *registry,const char *channel_id);
UmiStatus umi_context_registry_validate_payload(const UmiContextRegistry *registry,const char *channel_id,const UmiContextPayload *payload);
#ifdef __cplusplus
}
#endif
#endif
