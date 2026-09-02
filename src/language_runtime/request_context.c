/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/request_context.c
 *
 * PURPOSE:
 *   Implement bounded asynchronous request context retention.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/request_context.h"
#include <string.h>
/*
 * Initialise language runtime request context from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_runtime_request_context_init(UmiLanguageRuntimeRequestContextTable*t){/* Apply this branch only when its contract condition is satisfied. */ if(t)memset(t,0,sizeof(*t));}
/*
 * Add language runtime request context only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_language_runtime_request_context_add(UmiLanguageRuntimeRequestContextTable*t,const UmiLanguageRuntimeRequestContext*c){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||c==NULL||!c->request_id||!c->method[0])return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<UMI_LANGUAGE_RUNTIME_MAX_PENDING_REQUESTS;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t->items[i].active&&t->items[i].request_id==c->request_id)return UMI_STATUS_ALREADY_EXISTS;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<UMI_LANGUAGE_RUNTIME_MAX_PENDING_REQUESTS;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!t->items[i].active){t->items[i]=*c;t->items[i].sequence=++t->sequence;t->items[i].active=1;t->active_count++;return UMI_STATUS_OK;}return UMI_STATUS_CAPACITY_EXCEEDED;}
/*
 * Provide the language runtime request context take operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_request_context_take(UmiLanguageRuntimeRequestContextTable*t,uint64_t id,UmiLanguageRuntimeRequestContext*out){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||!id||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<UMI_LANGUAGE_RUNTIME_MAX_PENDING_REQUESTS;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t->items[i].active&&t->items[i].request_id==id){*out=t->items[i];memset(&t->items[i],0,sizeof(t->items[i]));t->active_count--;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
