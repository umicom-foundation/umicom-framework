/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/request_ledger.c
 *
 * PURPOSE:
 *   Implement the request ledger behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework LSP request ledger. Sammy Hegab, Umicom Foundation, MIT. */
#include "umicom/language/request_ledger.h"
#include <stdlib.h>
#include <string.h>
struct UmiLanguageRequestLedger{UmiLanguageRequestRecord items[UMI_LANGUAGE_REQUEST_LEDGER_CAPACITY];size_t count;uint64_t revision;};
/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiLanguageRequestLedger*l,int64_t id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l==NULL)return SIZE_MAX;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<l->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l->items[i].request_id==id)return i;return SIZE_MAX;}
/*
 * Initialise language request ledger from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_request_ledger_create(UmiLanguageRequestLedger**out){UmiLanguageRequestLedger*l;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;l=calloc(1U,sizeof(*l));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l==NULL)return UMI_STATUS_OUT_OF_MEMORY;l->revision=1U;*out=l;return UMI_STATUS_OK;}
/*
 * Release or reset state held by language request ledger so the same storage can be reused
 * safely.
 */
void umi_language_request_ledger_destroy(UmiLanguageRequestLedger*l){free(l);}
/*
 * Provide the language request ledger begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_request_ledger_begin(UmiLanguageRequestLedger*l,const UmiLanguageRequestRecord*r){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l==NULL||r==NULL||r->request_id<=0||r->method[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(find_index(l,r->request_id)!=SIZE_MAX)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l->count>=UMI_LANGUAGE_REQUEST_LEDGER_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;i=l->count++;l->items[i]=*r;l->items[i].method[127U]='\0';l->items[i].document_uri[1023U]='\0';l->items[i].state=UMI_LANGUAGE_REQUEST_PENDING;l->items[i].status=UMI_STATUS_BUSY;l->revision+=1U;l->items[i].revision=l->revision;return UMI_STATUS_OK;}
/*
 * Provide the language request ledger complete operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_request_ledger_complete(UmiLanguageRequestLedger*l,int64_t id,UmiStatus status,uint64_t completed){size_t i=find_index(l,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l==NULL||id<=0)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l->items[i].state!=UMI_LANGUAGE_REQUEST_PENDING)return UMI_STATUS_INVALID_STATE;l->items[i].completed_at=completed;l->items[i].status=status;l->items[i].state=status==UMI_STATUS_OK?UMI_LANGUAGE_REQUEST_COMPLETED:UMI_LANGUAGE_REQUEST_FAILED;l->revision+=1U;l->items[i].revision=l->revision;return UMI_STATUS_OK;}
/*
 * Find language request ledger while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_language_request_ledger_find(const UmiLanguageRequestLedger*l,int64_t id,UmiLanguageRequestRecord*out){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(l,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;*out=l->items[i];return UMI_STATUS_OK;}
/*
 * Find language request ledger while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_language_request_ledger_at(const UmiLanguageRequestLedger*l,size_t i,UmiLanguageRequestRecord*out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i>=l->count)return UMI_STATUS_NOT_FOUND;*out=l->items[i];return UMI_STATUS_OK;}
/*
 * Return the number of records represented by language request ledger without changing
 * their state.
 */
size_t umi_language_request_ledger_count(const UmiLanguageRequestLedger*l){return l!=NULL?l->count:0U;}
/*
 * Provide the language request ledger pending operation used by this module and its client
 * applications.
 */
size_t umi_language_request_ledger_pending(const UmiLanguageRequestLedger*l){size_t i,n=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l==NULL)return 0U;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<l->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l->items[i].state==UMI_LANGUAGE_REQUEST_PENDING)n+=1U;return n;}
