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
typedef struct M{char lang[128],root[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY];UmiLanguageRuntimeServer*s;UmiLanguageRuntimeInitializeResult caps;int ready;}M;struct UmiLanguageRuntimeServerManager{UmiLanguageService*l;M a[UMI_LANGUAGE_RUNTIME_MAX_SERVERS];size_t n;};
/*
 * Initialise language runtime server manager from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_language_runtime_server_manager_create(UmiLanguageService*l,UmiLanguageRuntimeServerManager**out){UmiLanguageRuntimeServerManager*m;UmiStatus q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!l||!out)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;m=calloc(1,sizeof(*m));/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!m)return UMI_STATUS_OUT_OF_MEMORY;m->l=l;q=umi_language_runtime_register_builtin_profiles(l);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK){free(m);return q;}*out=m;return UMI_STATUS_OK;}
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
UmiStatus umi_language_runtime_server_manager_attach(UmiLanguageRuntimeServerManager*m,const char*lang,const char*root,UmiLanguageRuntimeServer*s,const UmiLanguageRuntimeInitializeResult*caps){char n[128];M*x;UmiStatus q;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!m||!lang||!root||!s)return UMI_STATUS_INVALID_ARGUMENT;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(m->n>=UMI_LANGUAGE_RUNTIME_MAX_SERVERS)return UMI_STATUS_CAPACITY_EXCEEDED;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_language_runtime_normalize_language_id(lang,n,sizeof(n))!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_language_runtime_server_manager_find(m,n,root))return UMI_STATUS_ALREADY_EXISTS;x=&m->a[m->n];memset(x,0,sizeof(*x));snprintf(x->lang,sizeof(x->lang),"%s",n);snprintf(x->root,sizeof(x->root),"%s",root);x->s=s;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(caps){q=umi_language_runtime_server_transition(s,UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(q==UMI_STATUS_OK)q=umi_language_runtime_server_transition(s,UMI_LANGUAGE_RUNTIME_SERVER_READY);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(q!=UMI_STATUS_OK){memset(x,0,sizeof(*x));return q;}x->caps=*caps;x->ready=1;}m->n++;return UMI_STATUS_OK;}
/* Provide the init operation used by this module and its client applications. */
static UmiStatus init(UmiLanguageRuntimeServer*s,const char*root,uint32_t timeout,UmiLanguageRuntimeInitializeResult*out){uint64_t id=0;unsigned attempts=0;UmiStatus q=umi_language_runtime_server_transition(s,UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;q=umi_language_runtime_request_initialize(s,root,&id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(attempts++<64){UmiLanguageRuntimeEnvelope e;q=umi_language_runtime_server_receive(s,timeout,&e);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q==UMI_STATUS_NOT_FOUND)continue;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(e.request_id!=id)continue;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(e.kind==UMI_LANGUAGE_RUNTIME_MESSAGE_ERROR)return UMI_STATUS_UNAVAILABLE;q=umi_language_runtime_decode_initialize(e.json,out);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;q=umi_language_runtime_request_initialized(s);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;return umi_language_runtime_server_transition(s,UMI_LANGUAGE_RUNTIME_SERVER_READY);}return UMI_STATUS_TIMEOUT;}
/*
 * Provide the language runtime server manager start for language operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_runtime_server_manager_start_for_language(UmiLanguageRuntimeServerManager*m,const char*lang,const char*root,const char*wd,uint32_t timeout,UmiLanguageRuntimeServer**out){const UmiLanguageServerProfile*p;UmiLanguageRuntimeProfileHealth h;UmiLanguageRuntimeServer*s;UmiLanguageRuntimeInitializeResult caps={0};char n[128],id[192];UmiStatus q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!m||!lang||!root||!*root||!out)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_language_runtime_normalize_language_id(lang,n,sizeof(n))!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;s=umi_language_runtime_server_manager_find(m,n,root);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s&&umi_language_runtime_server_is_running(s)){*out=s;return UMI_STATUS_OK;}p=umi_language_runtime_builtin_profile_for_language(n);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!p)return UMI_STATUS_NOT_FOUND;q=umi_language_runtime_profile_health_probe(p,&h);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK||!h.available)return q!=UMI_STATUS_OK?q:UMI_STATUS_UNAVAILABLE;snprintf(id,sizeof(id),"%s.%zu",p->id,m->n+1);q=umi_language_runtime_server_start(id,p,root,wd,&s);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;q=init(s,root,timeout,&caps);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK){umi_language_runtime_server_destroy(s);return q;}q=umi_language_runtime_server_manager_attach(m,n,root,s,&caps);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK){umi_language_runtime_server_destroy(s);return q;}*out=s;return UMI_STATUS_OK;}
/*
 * Provide the language runtime server manager stop all operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_server_manager_stop_all(UmiLanguageRuntimeServerManager*m,uint32_t timeout){size_t i;UmiStatus first=UMI_STATUS_OK;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!m)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<m->n;i++){UmiLanguageRuntimeServer*s=m->a[i].s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s&&umi_language_runtime_server_is_running(s)){uint64_t id=0;UmiStatus q=umi_language_runtime_request_shutdown(s,&id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q==UMI_STATUS_OK){unsigned k=0;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(k++<16){UmiLanguageRuntimeEnvelope e;q=umi_language_runtime_server_receive(s,timeout,&e);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q==UMI_STATUS_NOT_FOUND)continue;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK||e.request_id==id)break;}/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q==UMI_STATUS_OK)q=umi_language_runtime_request_exit(s);}/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q==UMI_STATUS_OK)q=umi_language_runtime_server_stop(s,timeout);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(first==UMI_STATUS_OK&&q!=UMI_STATUS_OK)first=q;}}return first;}
/*
 * Return the number of records represented by language runtime server manager without
 * changing their state.
 */
size_t umi_language_runtime_server_manager_count(const UmiLanguageRuntimeServerManager*m){return m?m->n:0;}
