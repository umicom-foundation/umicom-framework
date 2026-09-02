/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/platform.c
 *
 * PURPOSE:
 *   Implement the complete Framework IntelliSense runtime composition and request API.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/platform.h"
#include "umicom/language/builtins.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct UmiLanguageRuntimePlatform{UmiLanguageService*l;UmiEditorSession*e;UmiLanguageRuntimeServerManager*m;UmiLanguageRuntimeDocumentSync*d;UmiLanguageRuntimeServiceBridge lb;UmiLanguageRuntimeEditorBridge eb;UmiLanguageRuntimeRequestContextTable ctx;UmiLanguageRuntimeDispatchState dispatch;uint64_t rev;};
/*
 * Initialise language runtime platform from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_runtime_platform_create(UmiLanguageRuntimePlatform**out){UmiLanguageRuntimePlatform*p;UmiStatus q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!out)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;p=calloc(1,sizeof(*p));/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!p)return UMI_STATUS_OUT_OF_MEMORY;q=umi_language_service_create(&p->l);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q==UMI_STATUS_OK)q=umi_language_register_builtin_definitions(umi_language_service_definition(p->l));/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q==UMI_STATUS_OK)q=umi_editor_session_create(&p->e);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q==UMI_STATUS_OK)q=umi_language_runtime_server_manager_create(p->l,&p->m);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q==UMI_STATUS_OK)q=umi_language_runtime_document_sync_create(p->l,&p->d);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q==UMI_STATUS_OK)q=umi_language_runtime_service_bridge_init(&p->lb,p->l);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q==UMI_STATUS_OK)q=umi_language_runtime_editor_bridge_init(&p->eb,p->l,p->e);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK){umi_language_runtime_platform_destroy(p);return q;}umi_language_runtime_request_context_init(&p->ctx);umi_language_runtime_dispatch_state_init(&p->dispatch);p->rev=1;*out=p;return UMI_STATUS_OK;}
/*
 * Release or reset state held by language runtime platform so the same storage can be
 * reused safely.
 */
void umi_language_runtime_platform_destroy(UmiLanguageRuntimePlatform*p){/* Apply this branch only when its contract condition is satisfied. */ if(!p)return;/* Apply this branch only when its contract condition is satisfied. */ if(p->m)(void)umi_language_runtime_server_manager_stop_all(p->m,100);umi_language_runtime_document_sync_destroy(p->d);umi_language_runtime_server_manager_destroy(p->m);umi_editor_session_destroy(p->e);umi_language_service_destroy(p->l);free(p);}
/*
 * Provide the language runtime platform attach server operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_platform_attach_server(UmiLanguageRuntimePlatform*p,const char*lang,const char*root,UmiLanguageRuntimeServer*s,const UmiLanguageRuntimeInitializeResult*caps){UmiStatus q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!p)return UMI_STATUS_INVALID_ARGUMENT;q=umi_language_runtime_server_manager_attach(p->m,lang,root,s,caps);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q==UMI_STATUS_OK)p->rev++;return q;}
/* Provide the doc operation used by this module and its client applications. */
static UmiStatus doc(UmiLanguageRuntimePlatform*p,const char*d,UmiLanguageRuntimeDocumentState*out){return p?umi_language_runtime_document_find(p->d,d,out):UMI_STATUS_INVALID_ARGUMENT;}
/*
 * Provide the language runtime platform open document operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_platform_open_document(UmiLanguageRuntimePlatform*p,const char*root,const char*wd,const char*d,const char*uri,const char*lang,const char*title,const char*text,uint32_t timeout){UmiLanguageRuntimeServer*s;UmiStatus q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!p||!root||!d||!uri||!lang||!title||!text)return UMI_STATUS_INVALID_ARGUMENT;s=umi_language_runtime_server_manager_find(p->m,lang,root);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!s){q=umi_language_runtime_server_manager_start_for_language(p->m,lang,root,wd,timeout,&s);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;}q=umi_language_runtime_document_open(p->d,s,d,uri,lang,text);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;q=umi_language_runtime_editor_bridge_sync_document(&p->eb,d,title,strlen(text));/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK){(void)umi_language_runtime_document_close(p->d,d);return q;}p->rev++;return UMI_STATUS_OK;}
/*
 * Provide the language runtime platform change document operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_platform_change_document(UmiLanguageRuntimePlatform*p,const char*d,const char*text){UmiEditorDocumentSnapshot e;UmiStatus q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!p||!d||!text)return UMI_STATUS_INVALID_ARGUMENT;q=umi_language_runtime_document_change(p->d,d,text);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;q=umi_editor_document_registry_find(umi_editor_session_document(p->e),d,&e);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;q=umi_language_runtime_editor_bridge_sync_document(&p->eb,d,e.title,strlen(text));/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q==UMI_STATUS_OK)p->rev++;return q;}
/*
 * Provide the language runtime platform save document operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_platform_save_document(UmiLanguageRuntimePlatform*p,const char*d,const char*text){UmiEditorDocumentSnapshot e;UmiStatus q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!p||!d)return UMI_STATUS_INVALID_ARGUMENT;q=umi_language_runtime_document_save(p->d,d,text);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;q=umi_editor_document_registry_find(umi_editor_session_document(p->e),d,&e);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;q=umi_language_runtime_editor_bridge_sync_document(&p->eb,d,e.title,text?strlen(text):e.byte_count);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q==UMI_STATUS_OK)p->rev++;return q;}
/*
 * Provide the language runtime platform close document operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_platform_close_document(UmiLanguageRuntimePlatform*p,const char*d){UmiStatus q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!p||!d)return UMI_STATUS_INVALID_ARGUMENT;q=umi_language_runtime_document_close(p->d,d);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;(void)umi_editor_document_registry_remove(umi_editor_session_document(p->e),d);p->rev++;return UMI_STATUS_OK;}
/* Provide the context operation used by this module and its client applications. */
static UmiStatus context(UmiLanguageRuntimePlatform*p,uint64_t id,const char*m,const UmiLanguageRuntimeDocumentState*d,uint32_t l,uint32_t c,const char*sid,const char*old,const char*arg){UmiLanguageRuntimeRequestContext x={0};x.request_id=id;snprintf(x.method,sizeof(x.method),"%s",m);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(d){snprintf(x.document_id,sizeof(x.document_id),"%s",d->document_id);snprintf(x.uri,sizeof(x.uri),"%s",d->uri);}snprintf(x.symbol_id,sizeof(x.symbol_id),"%s",sid?sid:"");snprintf(x.old_name,sizeof(x.old_name),"%s",old?old:"");snprintf(x.argument,sizeof(x.argument),"%s",arg?arg:"");x.line=l;x.column=c;return umi_language_runtime_request_context_add(&p->ctx,&x);}
typedef UmiStatus(*PR)(UmiLanguageRuntimeServer*,const char*,const char*,uint32_t,uint32_t,uint64_t*);
/* Provide the posreq operation used by this module and its client applications. */
static UmiStatus posreq(UmiLanguageRuntimePlatform*p,const char*d,uint32_t l,uint32_t c,const char*m,PR f){UmiLanguageRuntimeDocumentState x;uint64_t id=0;UmiStatus q=doc(p,d,&x);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;q=f(x.server,x.document_id,x.uri,l,c,&id);return q==UMI_STATUS_OK?context(p,id,m,&x,l,c,"","",""):q;}
/*
 * Provide the language runtime platform request completion operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_runtime_platform_request_completion(UmiLanguageRuntimePlatform*p,const char*d,uint32_t l,uint32_t c){return posreq(p,d,l,c,"textDocument/completion",umi_language_runtime_request_completion);}UmiStatus umi_language_runtime_platform_request_hover(UmiLanguageRuntimePlatform*p,const char*d,uint32_t l,uint32_t c){return posreq(p,d,l,c,"textDocument/hover",umi_language_runtime_request_hover);}UmiStatus umi_language_runtime_platform_request_signature(UmiLanguageRuntimePlatform*p,const char*d,uint32_t l,uint32_t c){return posreq(p,d,l,c,"textDocument/signatureHelp",umi_language_runtime_request_signature_help);}

/*
 * Provide the language runtime platform request definition operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_runtime_platform_request_definition(UmiLanguageRuntimePlatform*p,const char*d,const char*sid,uint32_t l,uint32_t c){UmiLanguageRuntimeDocumentState x;uint64_t id=0;UmiStatus q=doc(p,d,&x);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;q=umi_language_runtime_request_definition(x.server,x.document_id,x.uri,l,c,&id);return q==UMI_STATUS_OK?context(p,id,"textDocument/definition",&x,l,c,sid,"",""):q;}
/*
 * Provide the language runtime platform request references operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_runtime_platform_request_references(UmiLanguageRuntimePlatform*p,const char*d,const char*sid,uint32_t l,uint32_t c,int inc){UmiLanguageRuntimeDocumentState x;uint64_t id=0;UmiStatus q=doc(p,d,&x);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;q=umi_language_runtime_request_references(x.server,x.document_id,x.uri,l,c,inc,&id);return q==UMI_STATUS_OK?context(p,id,"textDocument/references",&x,l,c,sid,"",""):q;}
/*
 * Provide the language runtime platform request document symbols operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_runtime_platform_request_document_symbols(UmiLanguageRuntimePlatform*p,const char*d){UmiLanguageRuntimeDocumentState x;uint64_t id=0;UmiStatus q=doc(p,d,&x);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;q=umi_language_runtime_request_document_symbols(x.server,x.document_id,x.uri,&id);return q==UMI_STATUS_OK?context(p,id,"textDocument/documentSymbol",&x,0,0,"","",""):q;}
/*
 * Provide the language runtime platform request workspace symbols operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_runtime_platform_request_workspace_symbols(UmiLanguageRuntimePlatform*p,const char*lang,const char*root,const char*qstr){UmiLanguageRuntimeServer*s;uint64_t id=0;UmiStatus q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!p||!lang||!root||!qstr)return UMI_STATUS_INVALID_ARGUMENT;s=umi_language_runtime_server_manager_find(p->m,lang,root);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!s)return UMI_STATUS_NOT_FOUND;q=umi_language_runtime_request_workspace_symbols(s,qstr,&id);return q==UMI_STATUS_OK?context(p,id,"workspace/symbol",NULL,0,0,"","",qstr):q;}
/*
 * Provide the language runtime platform request code actions operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_runtime_platform_request_code_actions(UmiLanguageRuntimePlatform*p,const char*d,const UmiLanguageRuntimeRange*r){UmiLanguageRuntimeDocumentState x;uint64_t id=0;UmiStatus q=doc(p,d,&x);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;q=umi_language_runtime_request_code_action(x.server,x.document_id,x.uri,r,&id);return q==UMI_STATUS_OK?context(p,id,"textDocument/codeAction",&x,r?r->start.line:0,r?r->start.character:0,"","",""):q;}
/*
 * Provide the language runtime platform request formatting operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_runtime_platform_request_formatting(UmiLanguageRuntimePlatform*p,const char*d,uint32_t tab,int spaces){UmiLanguageRuntimeDocumentState x;uint64_t id=0;char a[64];UmiStatus q=doc(p,d,&x);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;q=umi_language_runtime_request_formatting(x.server,x.document_id,x.uri,tab,spaces,&id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;snprintf(a,sizeof(a),"%u:%d",tab,spaces!=0);return context(p,id,"textDocument/formatting",&x,0,0,"","",a);}
/*
 * Provide the language runtime platform request rename operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_platform_request_rename(UmiLanguageRuntimePlatform*p,const char*d,const char*sid,const char*old,const char*newn,uint32_t l,uint32_t c){UmiLanguageRuntimeDocumentState x;uint64_t id=0;UmiStatus q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!newn||!*newn)return UMI_STATUS_INVALID_ARGUMENT;q=doc(p,d,&x);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;q=umi_language_runtime_request_rename(x.server,x.document_id,x.uri,l,c,newn,&id);return q==UMI_STATUS_OK?context(p,id,"textDocument/rename",&x,l,c,sid,old,newn):q;}
/*
 * Provide the language runtime platform request semantic tokens operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_runtime_platform_request_semantic_tokens(UmiLanguageRuntimePlatform*p,const char*d){UmiLanguageRuntimeDocumentState x;uint64_t id=0;UmiStatus q=doc(p,d,&x);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;q=umi_language_runtime_request_semantic_tokens(x.server,x.document_id,x.uri,&id);return q==UMI_STATUS_OK?context(p,id,"textDocument/semanticTokens/full",&x,0,0,"","",""):q;}
/*
 * Provide the language runtime platform request inlay hints operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_runtime_platform_request_inlay_hints(UmiLanguageRuntimePlatform*p,const char*d,const UmiLanguageRuntimeRange*r){UmiLanguageRuntimeDocumentState x;uint64_t id=0;UmiStatus q=doc(p,d,&x);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;q=umi_language_runtime_request_inlay_hints(x.server,x.document_id,x.uri,r,&id);return q==UMI_STATUS_OK?context(p,id,"textDocument/inlayHint",&x,r?r->start.line:0,r?r->start.character:0,"","",""):q;}
/*
 * Provide the language runtime platform request folding ranges operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_runtime_platform_request_folding_ranges(UmiLanguageRuntimePlatform*p,const char*d){UmiLanguageRuntimeDocumentState x;uint64_t id=0;UmiStatus q=doc(p,d,&x);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;q=umi_language_runtime_request_folding_ranges(x.server,x.document_id,x.uri,&id);return q==UMI_STATUS_OK?context(p,id,"textDocument/foldingRange",&x,0,0,"","",""):q;}
/* Provide the pump operation used by this module and its client applications. */
static UmiStatus pump(UmiLanguageRuntimePlatform*p,UmiLanguageRuntimeServer*s,uint32_t tm,int*handled){UmiLanguageRuntimeEnvelope e;UmiLanguageRuntimeRequestContext c={0},*cp=NULL;UmiStatus q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!p||!s||!handled)return UMI_STATUS_INVALID_ARGUMENT;q=umi_language_runtime_server_receive(s,tm,&e);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(e.request_id&&umi_language_runtime_request_context_take(&p->ctx,e.request_id,&c)==UMI_STATUS_OK)cp=&c;q=umi_language_runtime_dispatch(&p->lb,&p->eb,p->d,&e,cp,&p->dispatch,handled);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q==UMI_STATUS_OK)p->rev++;return q;}
/*
 * Provide the language runtime platform pump document operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_platform_pump_document(UmiLanguageRuntimePlatform*p,const char*d,uint32_t tm,int*handled){UmiLanguageRuntimeDocumentState x;UmiStatus q=doc(p,d,&x);return q==UMI_STATUS_OK?pump(p,x.server,tm,handled):q;}
/*
 * Provide the language runtime platform pump server operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_platform_pump_server(UmiLanguageRuntimePlatform*p,const char*lang,const char*root,uint32_t tm,int*handled){UmiLanguageRuntimeServer*s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!p)return UMI_STATUS_INVALID_ARGUMENT;s=umi_language_runtime_server_manager_find(p->m,lang,root);return s?pump(p,s,tm,handled):UMI_STATUS_NOT_FOUND;}
/*
 * Provide the language runtime platform snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_platform_snapshot(UmiLanguageRuntimePlatform*p,UmiLanguageRuntimePlatformSnapshot*out){UmiStatus q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!p||!out)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof(*out));q=umi_language_service_snapshot(p->l,&out->language);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;q=umi_editor_session_snapshot(p->e,&out->editor);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;out->server_count=umi_language_runtime_server_manager_count(p->m);out->open_document_count=umi_language_runtime_document_sync_count(p->d);out->pending_context_count=p->ctx.active_count;out->dispatch=p->dispatch;out->revision=p->rev;return UMI_STATUS_OK;}
/*
 * Provide the language runtime platform language operation used by this module and its
 * client applications.
 */
UmiLanguageService *umi_language_runtime_platform_language(UmiLanguageRuntimePlatform*p){return p?p->l:NULL;}UmiEditorSession *umi_language_runtime_platform_editor(UmiLanguageRuntimePlatform*p){return p?p->e:NULL;}UmiLanguageRuntimeServerManager *umi_language_runtime_platform_servers(UmiLanguageRuntimePlatform*p){return p?p->m:NULL;}UmiLanguageRuntimeDocumentSync *umi_language_runtime_platform_documents(UmiLanguageRuntimePlatform*p){return p?p->d:NULL;}const UmiLanguageRuntimeDispatchState *umi_language_runtime_platform_dispatch_state(const UmiLanguageRuntimePlatform*p){return p?&p->dispatch:NULL;}
