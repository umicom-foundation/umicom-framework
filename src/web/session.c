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

/*
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/session.h"
#include <stdlib.h>
#include <string.h>
struct UmiWebSessionStore{UmiWebSession sessions[UMI_WEB_MAX_SESSIONS];size_t count;};
/*
 * Initialise web session store from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_web_session_store_create(UmiWebSessionStore **out_store){UmiWebSessionStore *s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_store==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out_store=NULL;s=(UmiWebSessionStore*)calloc(1U,sizeof(*s));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_OUT_OF_MEMORY;*out_store=s;return UMI_STATUS_OK;}
/*
 * Release or reset state held by web session store so the same storage can be reused
 * safely.
 */
void umi_web_session_store_destroy(UmiWebSessionStore *store){free(store);}
/* Provide the web session put operation used by this module and its client applications. */
UmiStatus umi_web_session_put(UmiWebSessionStore *store,const UmiWebSession *session){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(store==NULL||session==NULL||session->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<store->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(store->sessions[i].id,session->id)==0){store->sessions[i]=*session;return UMI_STATUS_OK;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(store->count>=UMI_WEB_MAX_SESSIONS)return UMI_STATUS_CAPACITY_EXCEEDED;store->sessions[store->count++]=*session;return UMI_STATUS_OK;}
/* Provide the web session get operation used by this module and its client applications. */
const UmiWebSession *umi_web_session_get(const UmiWebSessionStore *store,const char *id,uint64_t now_ns){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(store==NULL||id==NULL)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<store->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(store->sessions[i].id,id)==0&&store->sessions[i].active&&(store->sessions[i].expires_ns==0U||store->sessions[i].expires_ns>now_ns))return &store->sessions[i];return NULL;}
/*
 * Provide the web session revoke operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_session_revoke(UmiWebSessionStore *store,const char *id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(store==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<store->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(store->sessions[i].id,id)==0){store->sessions[i].active=0;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
/* Return the number of records represented by web session without changing their state. */
size_t umi_web_session_count(const UmiWebSessionStore *store){return store!=NULL?store->count:0U;}
