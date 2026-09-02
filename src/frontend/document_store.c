/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/document_store.c
 *
 * PURPOSE:
 *   Implement the document store behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/frontend/document_store.h"
#include <string.h>
/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiFrontendDocumentStore *s,const char *p){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||p==NULL)return SIZE_MAX;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<s->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(s->items[i].path,p)==0)return i;return SIZE_MAX;}
/*
 * Initialise frontend document store from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_frontend_document_store_init(UmiFrontendDocumentStore *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(s,0,sizeof(*s));s->revision=1U;return UMI_STATUS_OK;}
/*
 * Provide the frontend document store upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_document_store_upsert(UmiFrontendDocumentStore *s,const UmiFrontendDocument *d){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||d==NULL||d->path[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;i=find_index(s,d->path);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->count>=UMI_FRONTEND_DEV_MAX_DOCUMENTS)return UMI_STATUS_CAPACITY_EXCEEDED;i=s->count++;}s->items[i]=*d;s->items[i].revision=++s->revision;s->items[i].dirty=1;return UMI_STATUS_OK;}
/*
 * Find frontend document store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_frontend_document_store_find(const UmiFrontendDocumentStore *s,const char *p,UmiFrontendDocument *out){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||p==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(s,p);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;*out=s->items[i];return UMI_STATUS_OK;}
/*
 * Provide the frontend document store mark saved operation used by this module and its
 * client applications.
 */
UmiStatus umi_frontend_document_store_mark_saved(UmiFrontendDocumentStore *s,const char *p){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||p==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(s,p);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;s->items[i].dirty=0;s->items[i].revision=++s->revision;return UMI_STATUS_OK;}
