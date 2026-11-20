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
/* Retain the original state operations for review. The complete-record checks
 * below bound comparisons and enforce the creation and expiry interval. */
#if 0
/* Provide the web session put operation used by this module and its client applications. */
UmiStatus umi_web_session_put(UmiWebSessionStore *store,const UmiWebSession *session){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(store==NULL||session==NULL||session->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<store->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(store->sessions[i].id,session->id)==0){store->sessions[i]=*session;return UMI_STATUS_OK;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(store->count>=UMI_WEB_MAX_SESSIONS)return UMI_STATUS_CAPACITY_EXCEEDED;store->sessions[store->count++]=*session;return UMI_STATUS_OK;}
/* Provide the web session get operation used by this module and its client applications. */
const UmiWebSession *umi_web_session_get(const UmiWebSessionStore *store,const char *id,uint64_t now_ns){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(store==NULL||id==NULL)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<store->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(store->sessions[i].id,id)==0&&store->sessions[i].active&&(store->sessions[i].expires_ns==0U||store->sessions[i].expires_ns>now_ns))return &store->sessions[i];return NULL;}
/*
 * Provide the web session revoke operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_session_revoke(UmiWebSessionStore *store,const char *id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(store==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<store->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(store->sessions[i].id,id)==0){store->sessions[i].active=0;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
#endif

/* Both insert and lookup use the record's public ID capacity. A caller can
 * pass an ordinary short string or a fixed field awaiting validation. */
static UmiStatus web_session_id_status(const char *id)
{
    if (!id || !id[0]) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 1U; i < sizeof(((UmiWebSession *)0)->id); ++i) {
        if (id[i] == '\0') return UMI_STATUS_OK;
    }
    return UMI_STATUS_CAPACITY_EXCEEDED;
}

static size_t web_session_index(const UmiWebSessionStore *store, const char *id)
{
    for (size_t i = 0U; i < store->count; ++i) {
        if (strcmp(store->sessions[i].id, id) == 0) return i;
    }
    return store->count;
}

static int web_session_expired(const UmiWebSession *session, uint64_t now_ns)
{
    return session->expires_ns != 0U && now_ns >= session->expires_ns;
}

/* Admit a complete copied record before replacing an existing state entry. */
UmiStatus umi_web_session_put(UmiWebSessionStore *store, const UmiWebSession *session)
{
    if (!store || !session) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = web_session_id_status(session->id);
    if (status != UMI_STATUS_OK) return status;
    if (!memchr(session->principal, '\0', sizeof(session->principal))) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (session->expires_ns != 0U && session->expires_ns <= session->created_ns) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    const size_t index = web_session_index(store, session->id);
    if (index == UMI_WEB_MAX_SESSIONS) return UMI_STATUS_CAPACITY_EXCEEDED;
    store->sessions[index] = *session;
    if (index == store->count) ++store->count;
    return UMI_STATUS_OK;
}

const UmiWebSession *umi_web_session_get(const UmiWebSessionStore *store, const char *id, uint64_t now_ns)
{
    if (!store || web_session_id_status(id) != UMI_STATUS_OK) return NULL;
    const size_t index = web_session_index(store, id);
    if (index == store->count) return NULL;
    const UmiWebSession *session = &store->sessions[index];
    if (!session->active || now_ns < session->created_ns || web_session_expired(session, now_ns)) return NULL;
    return session;
}

UmiStatus umi_web_session_revoke(UmiWebSessionStore *store, const char *id)
{
    if (!store) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = web_session_id_status(id);
    if (status != UMI_STATUS_OK) return status;
    const size_t index = web_session_index(store, id);
    if (index == store->count) return UMI_STATUS_NOT_FOUND;
    store->sessions[index].active = 0;
    return UMI_STATUS_OK;
}

/* Keep reclamation explicit so the server owns the decision to advance time
 * and invalidate borrowed references. Move surviving records in place. */
size_t umi_web_session_prune(UmiWebSessionStore *store, uint64_t now_ns)
{
    if (!store) return 0U;
    size_t kept = 0U;
    for (size_t i = 0U; i < store->count; ++i) {
        const UmiWebSession *session = &store->sessions[i];
        if (!session->active || web_session_expired(session, now_ns)) continue;
        if (kept != i) store->sessions[kept] = *session;
        ++kept;
    }
    const size_t removed = store->count - kept;
    if (removed != 0U) memset(&store->sessions[kept], 0, removed * sizeof(store->sessions[0]));
    store->count = kept;
    return removed;
}
/* Return the number of records represented by web session without changing their state. */
size_t umi_web_session_count(const UmiWebSessionStore *store){return store!=NULL?store->count:0U;}
