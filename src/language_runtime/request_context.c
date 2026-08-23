/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/request_context.c
 *
 * PURPOSE:
 *   Implement bounded asynchronous request context retention.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/request_context.h"
#include <string.h>
void umi_language_runtime_request_context_init(UmiLanguageRuntimeRequestContextTable*t){if(t)memset(t,0,sizeof(*t));}
UmiStatus umi_language_runtime_request_context_add(UmiLanguageRuntimeRequestContextTable*t,const UmiLanguageRuntimeRequestContext*c){size_t i;if(t==NULL||c==NULL||!c->request_id||!c->method[0])return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<UMI_LANGUAGE_RUNTIME_MAX_PENDING_REQUESTS;i++)if(t->items[i].active&&t->items[i].request_id==c->request_id)return UMI_STATUS_ALREADY_EXISTS;for(i=0;i<UMI_LANGUAGE_RUNTIME_MAX_PENDING_REQUESTS;i++)if(!t->items[i].active){t->items[i]=*c;t->items[i].sequence=++t->sequence;t->items[i].active=1;t->active_count++;return UMI_STATUS_OK;}return UMI_STATUS_CAPACITY_EXCEEDED;}
UmiStatus umi_language_runtime_request_context_take(UmiLanguageRuntimeRequestContextTable*t,uint64_t id,UmiLanguageRuntimeRequestContext*out){size_t i;if(t==NULL||!id||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<UMI_LANGUAGE_RUNTIME_MAX_PENDING_REQUESTS;i++)if(t->items[i].active&&t->items[i].request_id==id){*out=t->items[i];memset(&t->items[i],0,sizeof(t->items[i]));t->active_count--;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
