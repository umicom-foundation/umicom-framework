/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/registry.c
 *
 * PURPOSE:
 *   Implement schema and channel registration with compatibility validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/registry.h"
#include <string.h>
/*
 * Initialise context registry from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_registry_init(UmiContextRegistry *registry){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(registry==NULL)return;
memset(registry,0,sizeof(*registry));
umi_context_schema_store_init(&registry->schemas);
umi_context_channel_store_init(&registry->channels);
registry->revision=1U;
}
/*
 * Provide the context registry register schema operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_registry_register_schema(UmiContextRegistry *registry,const UmiContextSchema *schema){
UmiStatus s;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(registry==NULL||schema==NULL)return UMI_STATUS_INVALID_ARGUMENT;
s=umi_context_schema_store_put(&registry->schemas,schema);
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(s==UMI_STATUS_OK)registry->revision+=1U;
return s;
}
/*
 * Provide the context registry register channel operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_registry_register_channel(UmiContextRegistry *registry,const UmiContextChannel *channel){
UmiStatus s;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(registry==NULL||channel==NULL)return UMI_STATUS_INVALID_ARGUMENT;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(umi_context_schema_store_find_const(&registry->schemas,channel->schema_id)==NULL)return UMI_STATUS_NOT_FOUND;
s=umi_context_channel_store_put(&registry->channels,channel);
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(s==UMI_STATUS_OK)registry->revision+=1U;
return s;
}
/*
 * Provide the context registry schema operation used by this module and its client
 * applications.
 */
const UmiContextSchema *umi_context_registry_schema(const UmiContextRegistry *registry,const char *schema_id){
return registry==NULL?NULL:umi_context_schema_store_find_const(&registry->schemas,schema_id);
}
/*
 * Provide the context registry channel operation used by this module and its client
 * applications.
 */
const UmiContextChannel *umi_context_registry_channel(const UmiContextRegistry *registry,const char *channel_id){
return registry==NULL?NULL:umi_context_channel_store_find_const(&registry->channels,channel_id);
}
/*
 * Provide the context registry validate payload operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_registry_validate_payload(const UmiContextRegistry *registry,const char *channel_id,const UmiContextPayload *payload){
const UmiContextChannel *channel;
const UmiContextSchema *schema;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(registry==NULL||channel_id==NULL||payload==NULL)return UMI_STATUS_INVALID_ARGUMENT;
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_context_payload_validate(payload)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
channel=umi_context_registry_channel(registry,channel_id);
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(channel==NULL||!channel->enabled)return UMI_STATUS_NOT_FOUND;
schema=umi_context_registry_schema(registry,channel->schema_id);
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(schema==NULL)return UMI_STATUS_NOT_FOUND;
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(strcmp(payload->identity.schema_id,schema->schema_id)!=0||payload->kind!=schema->kind)return UMI_STATUS_INVALID_ARGUMENT;
return UMI_STATUS_OK;
}
