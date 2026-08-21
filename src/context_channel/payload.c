/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/payload.c
 *
 * PURPOSE:
 *   Implement typed payload validation, values, integrity and expiry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/payload.h"
#include <string.h>
static uint64_t hash_bytes(uint64_t hash,const void *data,size_t size)
{
    const unsigned char *bytes=(const unsigned char *)data;
size_t i;
    for(i=0U;i<size;++i){
hash^=(uint64_t)bytes[i];
hash*=UINT64_C(1099511628211);
}
    return hash;
}
void umi_context_payload_init(UmiContextPayload *payload,UmiContextKind kind,const char *context_id,const char *schema_id)
{
    if (payload == NULL) {
        return;
    
}
    memset(payload, 0, sizeof(*payload));
    payload->structure_size = (uint32_t)sizeof(*payload);
    payload->kind = kind;
    if(context_id!=NULL)(void)umi_context_copy_text(payload->identity.context_id,sizeof(payload->identity.context_id),context_id);
    if(schema_id!=NULL)(void)umi_context_copy_text(payload->identity.schema_id,sizeof(payload->identity.schema_id),schema_id);
    switch(kind){
    case UMI_CONTEXT_KIND_SOURCE_LOCATION: umi_source_location_context_init(&payload->domain.source_location);
break;
    case UMI_CONTEXT_KIND_INSTRUMENT: umi_instrument_context_init(&payload->domain.instrument);
break;
    case UMI_CONTEXT_KIND_ACCOUNT: umi_account_context_init(&payload->domain.account);
break;
    case UMI_CONTEXT_KIND_TRADE: umi_trade_context_init(&payload->domain.trade);
break;
    case UMI_CONTEXT_KIND_PROJECT: umi_project_context_init(&payload->domain.project);
break;
    case UMI_CONTEXT_KIND_WORKSPACE: umi_workspace_context_init(&payload->domain.workspace);
break;
    case UMI_CONTEXT_KIND_MEDIA: umi_media_context_init(&payload->domain.media);
break;
    case UMI_CONTEXT_KIND_SELECTION: umi_selection_context_init(&payload->domain.selection);
break;
    default: break;
}
    umi_context_payload_refresh_hash(payload);
}
UmiStatus umi_context_payload_validate(const UmiContextPayload *payload)
{
    size_t i;
    if(payload==NULL||payload->structure_size!=sizeof(*payload))return UMI_STATUS_INVALID_ARGUMENT;
    if(payload->kind<UMI_CONTEXT_KIND_GENERIC||payload->kind>UMI_CONTEXT_KIND_SELECTION)return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(payload->identity.context_id,sizeof(payload->identity.context_id))||payload->identity.context_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(payload->identity.schema_id,sizeof(payload->identity.schema_id))||payload->identity.schema_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    if(payload->value_count>UMI_CONTEXT_MAX_VALUES)return UMI_STATUS_INVALID_ARGUMENT;
    for(i=0U;i<payload->value_count;++i)if(umi_context_value_validate(&payload->values[i])!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    switch(payload->kind){
    case UMI_CONTEXT_KIND_SOURCE_LOCATION:return umi_source_location_context_validate(&payload->domain.source_location);
    case UMI_CONTEXT_KIND_INSTRUMENT:return umi_instrument_context_validate(&payload->domain.instrument);
    case UMI_CONTEXT_KIND_ACCOUNT:return umi_account_context_validate(&payload->domain.account);
    case UMI_CONTEXT_KIND_TRADE:return umi_trade_context_validate(&payload->domain.trade);
    case UMI_CONTEXT_KIND_PROJECT:return umi_project_context_validate(&payload->domain.project);
    case UMI_CONTEXT_KIND_WORKSPACE:return umi_workspace_context_validate(&payload->domain.workspace);
    case UMI_CONTEXT_KIND_MEDIA:return umi_media_context_validate(&payload->domain.media);
    case UMI_CONTEXT_KIND_SELECTION:return umi_selection_context_validate(&payload->domain.selection);
    default:return UMI_STATUS_OK;
}
}
UmiStatus umi_context_payload_copy(UmiContextPayload *d,const UmiContextPayload *s)
{
if(d==NULL||s==NULL)return UMI_STATUS_INVALID_ARGUMENT;
if(umi_context_payload_validate(s)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
*d=*s;
return UMI_STATUS_OK;
}
UmiStatus umi_context_payload_add_value(UmiContextPayload *payload,const UmiContextValue *value)
{
    size_t i;
if(payload==NULL||value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
if(umi_context_value_validate(value)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    for(i=0U;i<payload->value_count;++i)if(strcmp(payload->values[i].name,value->name)==0){
payload->values[i]=*value;
umi_context_payload_refresh_hash(payload);
return UMI_STATUS_OK;
}
    if (payload->value_count >= UMI_CONTEXT_MAX_VALUES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    
}
    payload->values[payload->value_count++] = *value;
    umi_context_payload_refresh_hash(payload);
    return UMI_STATUS_OK;
}
const UmiContextValue *umi_context_payload_find_value(const UmiContextPayload *payload,const char *name)
{
size_t i;
if(payload==NULL||name==NULL)return NULL;
for(i=0U;i<payload->value_count;++i)if(strcmp(payload->values[i].name,name)==0)return &payload->values[i];
return NULL;
}
uint64_t umi_context_payload_calculate_hash(const UmiContextPayload *payload)
{
    uint64_t hash=UINT64_C(1469598103934665603);
size_t i;
if(payload==NULL)return 0U;
    hash=hash_bytes(hash,&payload->kind,sizeof(payload->kind));
hash=umi_context_hash_text(hash,payload->identity.context_id,sizeof(payload->identity.context_id));
hash=umi_context_hash_text(hash,payload->identity.schema_id,sizeof(payload->identity.schema_id));
    hash=umi_context_hash_text(hash,payload->identity.source_application_id,sizeof(payload->identity.source_application_id));
hash=umi_context_hash_text(hash,payload->identity.source_panel_id,sizeof(payload->identity.source_panel_id));
    for(i=0U;i<payload->value_count;++i){
const UmiContextValue *v=&payload->values[i];
hash=umi_context_hash_text(hash,v->name,sizeof(v->name));
hash=hash_bytes(hash,&v->kind,sizeof(v->kind));
hash=umi_context_hash_text(hash,v->text,sizeof(v->text));
hash=hash_bytes(hash,&v->integer_value,sizeof(v->integer_value));
hash=hash_bytes(hash,&v->unsigned_value,sizeof(v->unsigned_value));
hash=hash_bytes(hash,&v->decimal_value,sizeof(v->decimal_value));
hash=hash_bytes(hash,&v->boolean_value,sizeof(v->boolean_value));
}
    return hash;
}
void umi_context_payload_refresh_hash(UmiContextPayload *payload){
if(payload!=NULL)payload->content_hash=umi_context_payload_calculate_hash(payload);
}
bool umi_context_payload_is_expired(const UmiContextPayload *payload,uint64_t now_ms){
return payload!=NULL&&payload->audit.expires_at_ms!=0U&&now_ms>=payload->audit.expires_at_ms;
}
