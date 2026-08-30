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
void umi_context_registry_init(UmiContextRegistry *registry){
if(registry==NULL)return;
memset(registry,0,sizeof(*registry));
umi_context_schema_store_init(&registry->schemas);
umi_context_channel_store_init(&registry->channels);
registry->revision=1U;
}
UmiStatus umi_context_registry_register_schema(UmiContextRegistry *registry,const UmiContextSchema *schema){
UmiStatus s;
if(registry==NULL||schema==NULL)return UMI_STATUS_INVALID_ARGUMENT;
s=umi_context_schema_store_put(&registry->schemas,schema);
if(s==UMI_STATUS_OK)registry->revision+=1U;
return s;
}
UmiStatus umi_context_registry_register_channel(UmiContextRegistry *registry,const UmiContextChannel *channel){
UmiStatus s;
if(registry==NULL||channel==NULL)return UMI_STATUS_INVALID_ARGUMENT;
if(umi_context_schema_store_find_const(&registry->schemas,channel->schema_id)==NULL)return UMI_STATUS_NOT_FOUND;
s=umi_context_channel_store_put(&registry->channels,channel);
if(s==UMI_STATUS_OK)registry->revision+=1U;
return s;
}
const UmiContextSchema *umi_context_registry_schema(const UmiContextRegistry *registry,const char *schema_id){
return registry==NULL?NULL:umi_context_schema_store_find_const(&registry->schemas,schema_id);
}
const UmiContextChannel *umi_context_registry_channel(const UmiContextRegistry *registry,const char *channel_id){
return registry==NULL?NULL:umi_context_channel_store_find_const(&registry->channels,channel_id);
}
UmiStatus umi_context_registry_validate_payload(const UmiContextRegistry *registry,const char *channel_id,const UmiContextPayload *payload){
const UmiContextChannel *channel;
const UmiContextSchema *schema;
if(registry==NULL||channel_id==NULL||payload==NULL)return UMI_STATUS_INVALID_ARGUMENT;
if(umi_context_payload_validate(payload)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
channel=umi_context_registry_channel(registry,channel_id);
if(channel==NULL||!channel->enabled)return UMI_STATUS_NOT_FOUND;
schema=umi_context_registry_schema(registry,channel->schema_id);
if(schema==NULL)return UMI_STATUS_NOT_FOUND;
if(strcmp(payload->identity.schema_id,schema->schema_id)!=0||payload->kind!=schema->kind)return UMI_STATUS_INVALID_ARGUMENT;
return UMI_STATUS_OK;
}
