/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/server_manager.c
 *
 * PURPOSE:
 *   Implement server selection, executable health, initialize handshake, reuse and shutdown.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/server_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server_profile_internal.h"
typedef struct M{char lang[128],root[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY];UmiLanguageRuntimeServer*s;UmiLanguageRuntimeInitializeResult caps;int ready;}M;struct UmiLanguageRuntimeServerManager{UmiLanguageService*l;M a[UMI_LANGUAGE_RUNTIME_MAX_SERVERS];size_t n;};
/*
 * Initialise language runtime server manager from caller-provided values so later
 * operations receive a known state.
 */
/* Manager construction fills missing built-in profiles while preserving existing
 * user configuration instead of unconditionally replacing registry values.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_language_runtime_server_manager_create(UmiLanguageService*l,UmiLanguageRuntimeServerManager**out){UmiLanguageRuntimeServerManager*m;UmiStatus q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!l||!out)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;m=calloc(1,sizeof(*m));/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!m)return UMI_STATUS_OUT_OF_MEMORY;m->l=l;q=umi_language_runtime_register_builtin_profiles(l);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK){free(m);return q;}*out=m;return UMI_STATUS_OK;}
#endif
UmiStatus umi_language_runtime_server_manager_create(UmiLanguageService *language,
    UmiLanguageRuntimeServerManager **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (language == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiLanguageRuntimeServerManager *manager = calloc(1U, sizeof(*manager));
    if (manager == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    manager->l = language;
    UmiLanguageServerProfileRegistry *profiles = umi_language_service_server_profiles(language);
    UmiStatus status = profiles == NULL ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < umi_language_runtime_builtin_profile_count(); ++i)
    {
        const UmiLanguageServerProfile *builtin = umi_language_runtime_builtin_profile_at(i);
        UmiLanguageServerProfile existing;
        status = umi_language_server_profile_registry_find(profiles, builtin->id, &existing);
        if (status == UMI_STATUS_NOT_FOUND)
            status = umi_language_server_profile_registry_upsert(profiles, builtin);
        /* Existing user-selected executables, arguments and disabled state
         * belong to the language service. Creating an owner must not reset them. */
    }
    if (status != UMI_STATUS_OK) { free(manager); return status; }
    *out = manager;
    return UMI_STATUS_OK;
}
/*
 * Release or reset state held by language runtime server manager so the same storage can
 * be reused safely.
 */
void umi_language_runtime_server_manager_destroy(UmiLanguageRuntimeServerManager*m){size_t i;/* Apply this branch only when its contract condition is satisfied. */ if(!m)return;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<m->n;i++)umi_language_runtime_server_destroy(m->a[i].s);free(m);}
/*
 * Find language runtime server manager while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiLanguageRuntimeServer *umi_language_runtime_server_manager_find(UmiLanguageRuntimeServerManager*m,const char*lang,const char*root){char n[128];size_t i;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!m||!root||umi_language_runtime_normalize_language_id(lang,n,sizeof(n))!=UMI_STATUS_OK)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<m->n;i++)/* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(m->a[i].lang,n)==0&&strcmp(m->a[i].root,root)==0)return m->a[i].s;return NULL;}
/*
 * Provide the language runtime server manager attach operation used by this module and its
 * client applications.
 */
/* Attachment now validates ownership before publication and retains a completed
 * READY handshake instead of attempting an invalid backwards transition.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_language_runtime_server_manager_attach(UmiLanguageRuntimeServerManager*m,const char*lang,const char*root,UmiLanguageRuntimeServer*s,const UmiLanguageRuntimeInitializeResult*caps){char n[128];M*x;UmiStatus q;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!m||!lang||!root||!s)return UMI_STATUS_INVALID_ARGUMENT;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(m->n>=UMI_LANGUAGE_RUNTIME_MAX_SERVERS)return UMI_STATUS_CAPACITY_EXCEEDED;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_language_runtime_normalize_language_id(lang,n,sizeof(n))!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_language_runtime_server_manager_find(m,n,root))return UMI_STATUS_ALREADY_EXISTS;x=&m->a[m->n];memset(x,0,sizeof(*x));snprintf(x->lang,sizeof(x->lang),"%s",n);snprintf(x->root,sizeof(x->root),"%s",root);x->s=s;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(caps){q=umi_language_runtime_server_transition(s,UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(q==UMI_STATUS_OK)q=umi_language_runtime_server_transition(s,UMI_LANGUAGE_RUNTIME_SERVER_READY);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(q!=UMI_STATUS_OK){memset(x,0,sizeof(*x));return q;}x->caps=*caps;x->ready=1;}m->n++;return UMI_STATUS_OK;}
#endif
UmiStatus umi_language_runtime_server_manager_attach(UmiLanguageRuntimeServerManager *manager,
    const char *languageId, const char *root, UmiLanguageRuntimeServer *server,
    const UmiLanguageRuntimeInitializeResult *capabilities)
{
    if (manager == NULL || languageId == NULL || root == NULL || root[0] == '\0' || server == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    char normalized[128];
    UmiStatus status = umi_language_runtime_normalize_language_id(languageId, normalized, sizeof(normalized));
    if (status != UMI_STATUS_OK) return status;
    if (strlen(root) >= sizeof(manager->a[0].root)) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < manager->n; ++i)
        if (manager->a[i].s == server ||
            (strcmp(manager->a[i].lang, normalized) == 0 && strcmp(manager->a[i].root, root) == 0))
            return UMI_STATUS_ALREADY_EXISTS;
    if (manager->n >= UMI_LANGUAGE_RUNTIME_MAX_SERVERS) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiLanguageRuntimeServerSnapshot snapshot;
    status = umi_language_runtime_server_snapshot(server, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(snapshot.root_uri, root) != 0) return UMI_STATUS_INVALID_ARGUMENT;
    if (capabilities != NULL)
    {
        /* Native startup already completed its handshake. Memory transports
         * can still attach STARTING or INITIALIZING fixtures with supplied
         * capabilities; never transition a READY server backwards. */
        if (snapshot.state == UMI_LANGUAGE_RUNTIME_SERVER_STARTING)
            status = umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);
        else if (snapshot.state != UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING &&
            snapshot.state != UMI_LANGUAGE_RUNTIME_SERVER_READY) return UMI_STATUS_INVALID_STATE;
        if (status == UMI_STATUS_OK && snapshot.state != UMI_LANGUAGE_RUNTIME_SERVER_READY)
            status = umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_READY);
        if (status != UMI_STATUS_OK) return status;
    }
    M *entry = &manager->a[manager->n];
    memset(entry, 0, sizeof(*entry));
    memcpy(entry->lang, normalized, strlen(normalized) + 1U);
    memcpy(entry->root, root, strlen(root) + 1U);
    entry->s = server;
    if (capabilities != NULL) { entry->caps = *capabilities; entry->ready = 1; }
    ++manager->n;
    return UMI_STATUS_OK;
}
/* Provide the init operation used by this module and its client applications. */
/* The shared lifecycle coordinator replaces repeated full-timeout reads with
 * one deadline and verifies the kind of the matching response.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus init(UmiLanguageRuntimeServer*s,const char*root,uint32_t timeout,UmiLanguageRuntimeInitializeResult*out){uint64_t id=0;unsigned attempts=0;UmiStatus q=umi_language_runtime_server_transition(s,UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;q=umi_language_runtime_request_initialize(s,root,&id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(attempts++<64){UmiLanguageRuntimeEnvelope e;q=umi_language_runtime_server_receive(s,timeout,&e);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q==UMI_STATUS_NOT_FOUND)continue;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(e.request_id!=id)continue;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(e.kind==UMI_LANGUAGE_RUNTIME_MESSAGE_ERROR)return UMI_STATUS_UNAVAILABLE;q=umi_language_runtime_decode_initialize(e.json,out);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;q=umi_language_runtime_request_initialized(s);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;return umi_language_runtime_server_transition(s,UMI_LANGUAGE_RUNTIME_SERVER_READY);}return UMI_STATUS_TIMEOUT;}
#endif
static UmiStatus init(UmiLanguageRuntimeServer *server, const char *root,
    uint32_t timeout, UmiLanguageRuntimeInitializeResult *out)
{
    return UmiLanguageRuntimeServerInitialize(server, root, timeout, NULL, out);
}
/*
 * Provide the language runtime server manager start for language operation used by this
 * module and its client applications.
 */
/* Configured profile values now reach the native launcher, and existing server
 * lifetimes are never destroyed while document synchronization may borrow them.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_language_runtime_server_manager_start_for_language(UmiLanguageRuntimeServerManager*m,const char*lang,const char*root,const char*wd,uint32_t timeout,UmiLanguageRuntimeServer**out){const UmiLanguageServerProfile*p;UmiLanguageRuntimeProfileHealth h;UmiLanguageRuntimeServer*s;UmiLanguageRuntimeInitializeResult caps={0};char n[128],id[192];UmiStatus q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!m||!lang||!root||!*root||!out)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_language_runtime_normalize_language_id(lang,n,sizeof(n))!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;s=umi_language_runtime_server_manager_find(m,n,root);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s&&umi_language_runtime_server_is_running(s)){*out=s;return UMI_STATUS_OK;}p=umi_language_runtime_builtin_profile_for_language(n);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!p)return UMI_STATUS_NOT_FOUND;q=umi_language_runtime_profile_health_probe(p,&h);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK||!h.available)return q!=UMI_STATUS_OK?q:UMI_STATUS_UNAVAILABLE;snprintf(id,sizeof(id),"%s.%zu",p->id,m->n+1);q=umi_language_runtime_server_start(id,p,root,wd,&s);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;q=init(s,root,timeout,&caps);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK){umi_language_runtime_server_destroy(s);return q;}q=umi_language_runtime_server_manager_attach(m,n,root,s,&caps);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK){umi_language_runtime_server_destroy(s);return q;}*out=s;return UMI_STATUS_OK;}
#endif
UmiStatus umi_language_runtime_server_manager_start_for_language(UmiLanguageRuntimeServerManager *manager,
    const char *languageId, const char *root, const char *workingDirectory, uint32_t timeout,
    UmiLanguageRuntimeServer **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (manager == NULL || languageId == NULL || root == NULL || root[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiLanguageRuntimeServer *existing = umi_language_runtime_server_manager_find(manager, languageId, root);
    if (existing != NULL)
    {
        UmiLanguageRuntimeServerSnapshot snapshot;
        UmiStatus status = umi_language_runtime_server_snapshot(existing, &snapshot);
        if (status != UMI_STATUS_OK) return status;
        if (snapshot.state != UMI_LANGUAGE_RUNTIME_SERVER_READY ||
            !umi_language_runtime_server_is_running(existing)) return UMI_STATUS_INVALID_STATE;
        *out = existing;
        return UMI_STATUS_OK;
    }
    const UmiLanguageServerProfile *builtin = umi_language_runtime_builtin_profile_for_language(languageId);
    if (builtin == NULL) return UMI_STATUS_NOT_FOUND;
    UmiLanguageServerProfile selected;
    UmiStatus status = umi_language_server_profile_registry_find(
        umi_language_service_server_profiles(manager->l), builtin->id, &selected);
    if (status == UMI_STATUS_NOT_FOUND) selected = *builtin;
    else if (status != UMI_STATUS_OK) return status;
    return UmiLanguageRuntimeServerManagerStartProfile(manager, languageId, &selected,
        root, workingDirectory, timeout, NULL, out);
}
/*
 * Provide the language runtime server manager stop all operation used by this module and
 * its client applications.
 */
/* Protocol errors no longer bypass child cleanup. Every owned server is stopped
 * through the shared shutdown coordinator while the first error remains visible.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_language_runtime_server_manager_stop_all(UmiLanguageRuntimeServerManager*m,uint32_t timeout){size_t i;UmiStatus first=UMI_STATUS_OK;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!m)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<m->n;i++){UmiLanguageRuntimeServer*s=m->a[i].s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s&&umi_language_runtime_server_is_running(s)){uint64_t id=0;UmiStatus q=umi_language_runtime_request_shutdown(s,&id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q==UMI_STATUS_OK){unsigned k=0;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(k++<16){UmiLanguageRuntimeEnvelope e;q=umi_language_runtime_server_receive(s,timeout,&e);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q==UMI_STATUS_NOT_FOUND)continue;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK||e.request_id==id)break;}/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q==UMI_STATUS_OK)q=umi_language_runtime_request_exit(s);}/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q==UMI_STATUS_OK)q=umi_language_runtime_server_stop(s,timeout);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(first==UMI_STATUS_OK&&q!=UMI_STATUS_OK)first=q;}}return first;}
#endif
UmiStatus umi_language_runtime_server_manager_stop_all(UmiLanguageRuntimeServerManager *manager,
    uint32_t timeout)
{
    if (manager == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus first = UMI_STATUS_OK;
    for (size_t i = 0U; i < manager->n; ++i)
    {
        UmiStatus status = UmiLanguageRuntimeServerShutdown(manager->a[i].s, timeout);
        if (first == UMI_STATUS_OK && status != UMI_STATUS_OK) first = status;
    }
    return first;
}
/*
 * Return the number of records represented by language runtime server manager without
 * changing their state.
 */
size_t umi_language_runtime_server_manager_count(const UmiLanguageRuntimeServerManager*m){return m?m->n:0;}

/* A selected profile is checked before any child is created. The public value
 * struct can be supplied by an application, so do not assume its fixed arrays
 * have terminators merely because registry-generated values usually do. */
UmiStatus UmiLanguageRuntimeServerManagerStartProfile(UmiLanguageRuntimeServerManager *manager,
    const char *languageId, const UmiLanguageServerProfile *profile, const char *root,
    const char *workingDirectory, uint32_t timeoutMs, const UmiCancellationToken *cancel,
    UmiLanguageRuntimeServer **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (manager == NULL || languageId == NULL || profile == NULL || root == NULL || root[0] == '\0' ||
        LanguageProfileText(profile) != UMI_STATUS_OK || profile->executable[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (!profile->enabled) return UMI_STATUS_UNAVAILABLE;
    if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
    char normalized[128], id[UMI_LANGUAGE_RUNTIME_ID_CAPACITY];
    UmiStatus status = umi_language_runtime_normalize_language_id(languageId, normalized, sizeof(normalized));
    if (status != UMI_STATUS_OK) return status;
    if (strlen(root) >= sizeof(manager->a[0].root)) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_language_runtime_server_manager_find(manager, normalized, root) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    if (manager->n >= UMI_LANGUAGE_RUNTIME_MAX_SERVERS) return UMI_STATUS_CAPACITY_EXCEEDED;
    int written = snprintf(id, sizeof(id), "%s.%zu", profile->id, manager->n + 1U);
    if (written < 0 || (size_t)written >= sizeof(id)) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiLanguageRuntimeServer *server = NULL;
    UmiLanguageRuntimeInitializeResult capabilities = {0};
    status = umi_language_runtime_server_start(id, profile, root, workingDirectory, &server);
    if (status == UMI_STATUS_OK)
    {
        /* Keep the no-token helper available for legacy callers while the
         * explicit profile path can cooperate with a worker's cancellation. */
        status = cancel == NULL ? init(server, root, timeoutMs, &capabilities) :
            UmiLanguageRuntimeServerInitialize(server, root, timeoutMs, cancel, &capabilities);
    }
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_manager_attach(manager, normalized, root, server, &capabilities);
    if (status != UMI_STATUS_OK)
    {
        umi_language_runtime_server_destroy(server);
        return status;
    }
    *out = server;
    return UMI_STATUS_OK;
}
