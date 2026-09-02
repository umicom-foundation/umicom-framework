/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/pending.c
 *
 * PURPOSE:
 *   Implement bounded outstanding request correlation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/pending.h"
#include <stdio.h>
#include <string.h>
/*
 * Initialise language runtime pending from caller-provided values so later operations
 * receive a known state.
 */
void umi_language_runtime_pending_init(UmiLanguageRuntimePendingTable*t){/* Apply this branch only when its contract condition is satisfied. */ if(t)memset(t,0,sizeof(*t));}
/*
 * Add language runtime pending only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_language_runtime_pending_add(UmiLanguageRuntimePendingTable*t,uint64_t id,const char*m,const char*d){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||!id||m==NULL||!*m)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<UMI_LANGUAGE_RUNTIME_MAX_PENDING_REQUESTS;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t->items[i].active&&t->items[i].request_id==id)return UMI_STATUS_ALREADY_EXISTS;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<UMI_LANGUAGE_RUNTIME_MAX_PENDING_REQUESTS;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!t->items[i].active){UmiLanguageRuntimePendingRequest*x=&t->items[i];memset(x,0,sizeof(*x));x->request_id=id;snprintf(x->method,sizeof(x->method),"%s",m);snprintf(x->document_id,sizeof(x->document_id),"%s",d?d:"");x->sequence=++t->sequence;x->active=1;t->active_count++;return UMI_STATUS_OK;}return UMI_STATUS_CAPACITY_EXCEEDED;}
/*
 * Find language runtime pending while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_language_runtime_pending_find(const UmiLanguageRuntimePendingTable*t,uint64_t id,UmiLanguageRuntimePendingRequest*out){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||!id||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<UMI_LANGUAGE_RUNTIME_MAX_PENDING_REQUESTS;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t->items[i].active&&t->items[i].request_id==id){*out=t->items[i];return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
/*
 * Provide the language runtime pending take operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_pending_take(UmiLanguageRuntimePendingTable*t,uint64_t id,UmiLanguageRuntimePendingRequest*out){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||!id||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<UMI_LANGUAGE_RUNTIME_MAX_PENDING_REQUESTS;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t->items[i].active&&t->items[i].request_id==id){*out=t->items[i];memset(&t->items[i],0,sizeof(t->items[i]));t->active_count--;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
