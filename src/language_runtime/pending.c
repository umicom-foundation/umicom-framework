/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/pending.c
 *
 * PURPOSE:
 *   Implement bounded outstanding request correlation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/pending.h"
#include <stdio.h>
#include <string.h>
void umi_language_runtime_pending_init(UmiLanguageRuntimePendingTable*t){if(t)memset(t,0,sizeof(*t));}
UmiStatus umi_language_runtime_pending_add(UmiLanguageRuntimePendingTable*t,uint64_t id,const char*m,const char*d){size_t i;if(t==NULL||!id||m==NULL||!*m)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<UMI_LANGUAGE_RUNTIME_MAX_PENDING_REQUESTS;i++)if(t->items[i].active&&t->items[i].request_id==id)return UMI_STATUS_ALREADY_EXISTS;for(i=0;i<UMI_LANGUAGE_RUNTIME_MAX_PENDING_REQUESTS;i++)if(!t->items[i].active){UmiLanguageRuntimePendingRequest*x=&t->items[i];memset(x,0,sizeof(*x));x->request_id=id;snprintf(x->method,sizeof(x->method),"%s",m);snprintf(x->document_id,sizeof(x->document_id),"%s",d?d:"");x->sequence=++t->sequence;x->active=1;t->active_count++;return UMI_STATUS_OK;}return UMI_STATUS_CAPACITY_EXCEEDED;}
UmiStatus umi_language_runtime_pending_find(const UmiLanguageRuntimePendingTable*t,uint64_t id,UmiLanguageRuntimePendingRequest*out){size_t i;if(t==NULL||!id||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<UMI_LANGUAGE_RUNTIME_MAX_PENDING_REQUESTS;i++)if(t->items[i].active&&t->items[i].request_id==id){*out=t->items[i];return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
UmiStatus umi_language_runtime_pending_take(UmiLanguageRuntimePendingTable*t,uint64_t id,UmiLanguageRuntimePendingRequest*out){size_t i;if(t==NULL||!id||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<UMI_LANGUAGE_RUNTIME_MAX_PENDING_REQUESTS;i++)if(t->items[i].active&&t->items[i].request_id==id){*out=t->items[i];memset(&t->items[i],0,sizeof(t->items[i]));t->active_count--;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
