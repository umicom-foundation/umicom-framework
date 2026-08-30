/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/session.c
 *
 * PURPOSE:
 *   Implement a revocable in-memory web session store.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/session.h"
#include <stdlib.h>
#include <string.h>
struct UmiWebSessionStore{UmiWebSession sessions[UMI_WEB_MAX_SESSIONS];size_t count;};
UmiStatus umi_web_session_store_create(UmiWebSessionStore **out_store){UmiWebSessionStore *s;if(out_store==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out_store=NULL;s=(UmiWebSessionStore*)calloc(1U,sizeof(*s));if(s==NULL)return UMI_STATUS_OUT_OF_MEMORY;*out_store=s;return UMI_STATUS_OK;}
void umi_web_session_store_destroy(UmiWebSessionStore *store){free(store);}
UmiStatus umi_web_session_put(UmiWebSessionStore *store,const UmiWebSession *session){size_t i;if(store==NULL||session==NULL||session->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<store->count;++i)if(strcmp(store->sessions[i].id,session->id)==0){store->sessions[i]=*session;return UMI_STATUS_OK;}if(store->count>=UMI_WEB_MAX_SESSIONS)return UMI_STATUS_CAPACITY_EXCEEDED;store->sessions[store->count++]=*session;return UMI_STATUS_OK;}
const UmiWebSession *umi_web_session_get(const UmiWebSessionStore *store,const char *id,uint64_t now_ns){size_t i;if(store==NULL||id==NULL)return NULL;for(i=0U;i<store->count;++i)if(strcmp(store->sessions[i].id,id)==0&&store->sessions[i].active&&(store->sessions[i].expires_ns==0U||store->sessions[i].expires_ns>now_ns))return &store->sessions[i];return NULL;}
UmiStatus umi_web_session_revoke(UmiWebSessionStore *store,const char *id){size_t i;if(store==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<store->count;++i)if(strcmp(store->sessions[i].id,id)==0){store->sessions[i].active=0;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
size_t umi_web_session_count(const UmiWebSessionStore *store){return store!=NULL?store->count:0U;}
