/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/server.c
 *
 * PURPOSE:
 *   Implement framed JSON-RPC I/O, request correlation and client lifecycle accounting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/clock.h"
#include "server_profile_internal.h"
struct UmiLanguageRuntimeServer{char id[UMI_LANGUAGE_RUNTIME_ID_CAPACITY];UmiLanguageServerProfile profile;char root[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY];UmiLanguageRuntimeTransport t;UmiLanguageRuntimeFramer f;UmiLanguageRuntimePendingTable p;UmiLanguageClientSession session;UmiLanguageRuntimeServerState state;uint64_t next,sent,recv,bs,br,rev;};
/* Provide the st operation used by this module and its client applications. */
static UmiStatus st(UmiLanguageRuntimeServer*s,UmiLanguageRuntimeServerState x){UmiLanguageClientState y;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!s)return UMI_STATUS_INVALID_ARGUMENT;/* Select the behaviour associated with the requested command or state value. */ switch(x){case UMI_LANGUAGE_RUNTIME_SERVER_STOPPED:y=UMI_LANGUAGE_CLIENT_STOPPED;break;case UMI_LANGUAGE_RUNTIME_SERVER_STARTING:y=UMI_LANGUAGE_CLIENT_STARTING;break;case UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING:y=UMI_LANGUAGE_CLIENT_INITIALIZING;break;case UMI_LANGUAGE_RUNTIME_SERVER_READY:y=UMI_LANGUAGE_CLIENT_READY;break;case UMI_LANGUAGE_RUNTIME_SERVER_STOPPING:y=UMI_LANGUAGE_CLIENT_STOPPING;break;default:y=UMI_LANGUAGE_CLIENT_FAILED;break;}/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s->session.state!=y){UmiStatus q=umi_language_client_session_transition(&s->session,y);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;}s->state=x;s->rev++;return UMI_STATUS_OK;}
/*
 * Provide the language runtime server create with transport operation used by this module
 * and its client applications.
 */
/* Bounded identity validation and final-step ownership transfer replace truncated
 * server identities and transport consumption during a failed constructor.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_language_runtime_server_create_with_transport(const char*id,const UmiLanguageServerProfile*p,const char*root,UmiLanguageRuntimeTransport*t,UmiLanguageRuntimeServer**out){UmiLanguageRuntimeServer*s;UmiStatus q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!id||!*id||!p||!p->id[0]||!root||!*root||!t||!out)return UMI_STATUS_INVALID_ARGUMENT;q=umi_language_runtime_transport_validate(t);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;*out=NULL;s=calloc(1,sizeof(*s));/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!s)return UMI_STATUS_OUT_OF_MEMORY;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(snprintf(s->id,sizeof(s->id),"%s",id)<0||snprintf(s->root,sizeof(s->root),"%s",root)<0){free(s);return UMI_STATUS_CAPACITY_EXCEEDED;}s->profile=*p;s->t=*t;memset(t,0,sizeof(*t));umi_language_runtime_framer_init(&s->f);umi_language_runtime_pending_init(&s->p);s->next=1;s->rev=1;q=umi_language_client_session_init(&s->session,id,p->id,root);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q==UMI_STATUS_OK)q=st(s,UMI_LANGUAGE_RUNTIME_SERVER_STARTING);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK){s->t.destroy(s->t.instance);free(s);return q;}*out=s;return UMI_STATUS_OK;}
#endif
UmiStatus umi_language_runtime_server_create_with_transport(const char *id,
    const UmiLanguageServerProfile *profile, const char *root, UmiLanguageRuntimeTransport *transport,
    UmiLanguageRuntimeServer **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (id == NULL || id[0] == '\0' || root == NULL || root[0] == '\0' ||
        LanguageProfileText(profile) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_language_runtime_transport_validate(transport);
    if (status != UMI_STATUS_OK) return status;
    UmiLanguageRuntimeServer *server = calloc(1U, sizeof(*server));
    if (server == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (strlen(id) >= sizeof(server->id) || strlen(root) >= sizeof(server->root))
    { free(server); return UMI_STATUS_CAPACITY_EXCEEDED; }
    memcpy(server->id, id, strlen(id) + 1U);
    memcpy(server->root, root, strlen(root) + 1U);
    server->profile = *profile;
    umi_language_runtime_framer_init(&server->f);
    umi_language_runtime_pending_init(&server->p);
    server->next = 1U; server->rev = 1U;
    status = umi_language_client_session_init(&server->session, id, profile->id, root);
    if (status == UMI_STATUS_OK) status = st(server, UMI_LANGUAGE_RUNTIME_SERVER_STARTING);
    if (status != UMI_STATUS_OK) { free(server); return status; }
    /* Only this final step transfers ownership. Failed validation leaves the
     * original transport usable and avoids double destruction in the caller. */
    server->t = *transport;
    memset(transport, 0, sizeof(*transport));
    *out = server;
    return UMI_STATUS_OK;
}
/*
 * Provide the language runtime server start operation used by this module and its client
 * applications.
 */
/* Validate profile strings and session identity before starting a native tool.
 * The same constructor now transfers transport ownership only on success.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_language_runtime_server_start(const char*id,const UmiLanguageServerProfile*p,const char*root,const char*wd,UmiLanguageRuntimeServer**out){UmiLanguageRuntimeArguments a;UmiLanguageRuntimeProcessStreamConfig c={0};UmiLanguageRuntimeProcessStream*ps=NULL;UmiLanguageRuntimeTransport t;UmiStatus q;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!p||!p->executable[0])return UMI_STATUS_INVALID_ARGUMENT;q=umi_language_runtime_arguments_parse(p->arguments,&a);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(q!=UMI_STATUS_OK)return q;c.program=p->executable;c.arguments=a.values;c.argument_count=a.count;c.working_directory=wd;q=umi_language_runtime_process_stream_start(&c,&ps);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(q!=UMI_STATUS_OK)return q;q=umi_language_runtime_transport_from_process(ps,&t);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(q!=UMI_STATUS_OK){umi_language_runtime_process_stream_destroy(ps);return q;}q=umi_language_runtime_server_create_with_transport(id,p,root,&t,out);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(q!=UMI_STATUS_OK&&t.instance)t.destroy(t.instance);return q;}
#endif
UmiStatus umi_language_runtime_server_start(const char *id, const UmiLanguageServerProfile *profile,
    const char *root, const char *workingDirectory, UmiLanguageRuntimeServer **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (id == NULL || id[0] == '\0' || root == NULL || root[0] == '\0' ||
        LanguageProfileText(profile) != UMI_STATUS_OK || profile->executable[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Reject identities that the session cannot hold before executing a tool. */
    UmiLanguageClientSession session;
    UmiStatus status = umi_language_client_session_init(&session, id, profile->id, root);
    if (status != UMI_STATUS_OK) return status;
    UmiLanguageRuntimeArguments arguments;
    status = umi_language_runtime_arguments_parse(profile->arguments, &arguments);
    if (status != UMI_STATUS_OK) return status;
    UmiLanguageRuntimeProcessStreamConfig config = {profile->executable, arguments.values,
        arguments.count, workingDirectory, 0};
    UmiLanguageRuntimeProcessStream *process = NULL;
    status = umi_language_runtime_process_stream_start(&config, &process);
    if (status != UMI_STATUS_OK) return status;
    UmiLanguageRuntimeTransport transport = {0};
    status = umi_language_runtime_transport_from_process(process, &transport);
    if (status != UMI_STATUS_OK) { umi_language_runtime_process_stream_destroy(process); return status; }
    status = umi_language_runtime_server_create_with_transport(id, profile, root, &transport, out);
    if (status != UMI_STATUS_OK && transport.instance != NULL) transport.destroy(transport.instance);
    return status;
}
/*
 * Release or reset state held by language runtime server so the same storage can be reused
 * safely.
 */
void umi_language_runtime_server_destroy(UmiLanguageRuntimeServer*s){/* Apply this branch only when its contract condition is satisfied. */ if(!s)return;/* Apply this branch only when its contract condition is satisfied. */ if(s->t.instance)s->t.destroy(s->t.instance);free(s);}
/*
 * Provide the language runtime server transition operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_server_transition(UmiLanguageRuntimeServer*s,UmiLanguageRuntimeServerState x){return x>=UMI_LANGUAGE_RUNTIME_SERVER_STOPPED&&x<=UMI_LANGUAGE_RUNTIME_SERVER_FAILED?st(s,x):UMI_STATUS_INVALID_ARGUMENT;}
/* Provide the sendj operation used by this module and its client applications. */
static UmiStatus sendj(UmiLanguageRuntimeServer*s,const char*j){char fr[UMI_LANGUAGE_RUNTIME_FRAME_CAPACITY];size_t n=0;UmiStatus q=umi_language_runtime_frame_encode(j,fr,sizeof(fr),&n);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;q=s->t.write(s->t.instance,fr,n);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q==UMI_STATUS_OK){s->sent++;s->bs+=n;s->rev++;}return q;}
/*
 * Provide the language runtime server send request operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_server_send_request(UmiLanguageRuntimeServer*s,const char*m,const char*p,const char*d,uint64_t*out){char j[UMI_LANGUAGE_RUNTIME_JSON_CAPACITY];uint64_t id;UmiStatus q;UmiLanguageRuntimePendingRequest ignored;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!s||!m)return UMI_STATUS_INVALID_ARGUMENT;id=s->next++;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!id)id=s->next++;q=umi_language_runtime_build_request(id,m,p,j,sizeof(j));/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;q=umi_language_runtime_pending_add(&s->p,id,m,d);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;q=sendj(s,j);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK){(void)umi_language_runtime_pending_take(&s->p,id,&ignored);return q;}umi_language_client_session_record_request(&s->session);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(out)*out=id;return UMI_STATUS_OK;}
/*
 * Provide the language runtime server send notification operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_server_send_notification(UmiLanguageRuntimeServer*s,const char*m,const char*p){char j[UMI_LANGUAGE_RUNTIME_JSON_CAPACITY];UmiStatus q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!s||!m)return UMI_STATUS_INVALID_ARGUMENT;q=umi_language_runtime_build_notification(m,p,j,sizeof(j));return q==UMI_STATUS_OK?sendj(s,j):q;}
/*
 * Provide the language runtime server receive operation used by this module and its client
 * applications.
 */
/* A single elapsed-time budget replaces per-chunk waits. Heap-owned decoding also
 * leaves the caller's envelope unchanged when a frame cannot be parsed.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_language_runtime_server_receive(UmiLanguageRuntimeServer*s,uint32_t tm,UmiLanguageRuntimeEnvelope*out){unsigned char b[8192];size_t n=0,jn=0;char j[UMI_LANGUAGE_RUNTIME_JSON_CAPACITY];UmiStatus q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!s||!out)return UMI_STATUS_INVALID_ARGUMENT;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(!umi_language_runtime_framer_has_message(&s->f)){q=s->t.read(s->t.instance,b,sizeof(b),tm,&n);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;s->br+=n;q=umi_language_runtime_framer_feed(&s->f,b,n);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!n)return UMI_STATUS_NOT_FOUND;}q=umi_language_runtime_framer_pop(&s->f,j,sizeof(j),&jn);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;(void)jn;q=umi_language_runtime_message_parse(j,out);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if((out->kind==UMI_LANGUAGE_RUNTIME_MESSAGE_RESPONSE||out->kind==UMI_LANGUAGE_RUNTIME_MESSAGE_ERROR)&&out->request_id){UmiLanguageRuntimePendingRequest x;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_language_runtime_pending_take(&s->p,out->request_id,&x)==UMI_STATUS_OK)snprintf(out->method,sizeof(out->method),"%s",x.method);umi_language_client_session_record_response(&s->session);}else /* Preserve the original failure result so the caller can respond to the correct cause. */ if(out->kind==UMI_LANGUAGE_RUNTIME_MESSAGE_NOTIFICATION)umi_language_client_session_record_notification(&s->session);s->recv++;s->rev++;return UMI_STATUS_OK;}
#endif
UmiStatus umi_language_runtime_server_receive(UmiLanguageRuntimeServer *server,
    uint32_t timeoutMs, UmiLanguageRuntimeEnvelope *out)
{
    if (server == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiClock clock = umi_clock_system();
    uint64_t start = clock.monotonic_nanoseconds(&clock);
    unsigned char bytes[8192];
    unsigned reads = 0U;
    /* A timeout covers the whole receive operation, including a fragmented
     * frame. Retain partial framing for the next call instead of restarting
     * the full wait for every arriving chunk. The byte budget also bounds a
     * custom nonblocking transport that returns tiny chunks without yielding. */
    while (!umi_language_runtime_framer_has_message(&server->f))
    {
        uint64_t elapsed = (clock.monotonic_nanoseconds(&clock) - start) / 1000000U;
        if ((reads != 0U && timeoutMs != 0U && elapsed >= timeoutMs) ||
            reads >= UMI_LANGUAGE_RUNTIME_FRAME_CAPACITY) return UMI_STATUS_NOT_FOUND;
        uint32_t remaining = elapsed >= timeoutMs ? 0U : timeoutMs - (uint32_t)elapsed;
        size_t count = 0U;
        UmiStatus status = server->t.read(server->t.instance, bytes, sizeof(bytes), remaining, &count);
        ++reads;
        if (status != UMI_STATUS_OK) return status;
        if (count > sizeof(bytes)) return UMI_STATUS_INVALID_STATE;
        if (count == 0U) return UMI_STATUS_NOT_FOUND;
        server->br += count;
        status = umi_language_runtime_framer_feed(&server->f, bytes, count);
        if (status != UMI_STATUS_OK) return status;
    }
    char *json = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    UmiLanguageRuntimeEnvelope *candidate = malloc(sizeof(*candidate));
    if (json == NULL || candidate == NULL)
    { free(json); free(candidate); return UMI_STATUS_OUT_OF_MEMORY; }
    size_t length = 0U;
    UmiStatus status = umi_language_runtime_framer_pop(&server->f, json,
        UMI_LANGUAGE_RUNTIME_JSON_CAPACITY, &length);
    if (status == UMI_STATUS_OK) status = umi_language_runtime_message_parse(json, candidate);
    free(json);
    if (status != UMI_STATUS_OK) { free(candidate); return status; }
    if ((candidate->kind == UMI_LANGUAGE_RUNTIME_MESSAGE_RESPONSE ||
        candidate->kind == UMI_LANGUAGE_RUNTIME_MESSAGE_ERROR) && candidate->request_id != 0U)
    {
        UmiLanguageRuntimePendingRequest pending;
        if (umi_language_runtime_pending_take(&server->p, candidate->request_id, &pending) == UMI_STATUS_OK)
            (void)snprintf(candidate->method, sizeof(candidate->method), "%s", pending.method);
        umi_language_client_session_record_response(&server->session);
    }
    else if (candidate->kind == UMI_LANGUAGE_RUNTIME_MESSAGE_NOTIFICATION)
        umi_language_client_session_record_notification(&server->session);
    ++server->recv; ++server->rev;
    *out = *candidate;
    free(candidate);
    return UMI_STATUS_OK;
}
/*
 * Provide the language runtime server stop operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_server_stop(UmiLanguageRuntimeServer*s,uint32_t tm){UmiStatus q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!s)return UMI_STATUS_INVALID_ARGUMENT;(void)st(s,UMI_LANGUAGE_RUNTIME_SERVER_STOPPING);q=s->t.stop(s->t.instance,tm);(void)st(s,q==UMI_STATUS_OK?UMI_LANGUAGE_RUNTIME_SERVER_STOPPED:UMI_LANGUAGE_RUNTIME_SERVER_FAILED);return q;}
/*
 * Provide the language runtime server is running operation used by this module and its
 * client applications.
 */
int umi_language_runtime_server_is_running(UmiLanguageRuntimeServer*s){return s&&s->t.instance&&s->t.is_running(s->t.instance);}
/*
 * Provide the language runtime server snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_server_snapshot(const UmiLanguageRuntimeServer*s,UmiLanguageRuntimeServerSnapshot*out){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!s||!out)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof(*out));snprintf(out->server_id,sizeof(out->server_id),"%s",s->id);snprintf(out->profile_id,sizeof(out->profile_id),"%s",s->profile.id);snprintf(out->root_uri,sizeof(out->root_uri),"%s",s->root);out->state=s->state;out->next_request_id=s->next;out->pending_requests=s->p.active_count;out->messages_sent=s->sent;out->messages_received=s->recv;out->bytes_sent=s->bs;out->bytes_received=s->br;out->revision=s->rev;return UMI_STATUS_OK;}
