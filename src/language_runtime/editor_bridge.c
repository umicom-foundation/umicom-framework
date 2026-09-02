/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/editor_bridge.c
 *
 * PURPOSE:
 *   Project authoritative Language Service state into established Editor Session registries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/editor_bridge.h"
#include <stdio.h>
#include <string.h>
/* Provide the same operation used by this module and its client applications. */
static int same(const char*a,const char*b){return a&&b&&strcmp(a,b)==0;}
/*
 * Initialise language runtime editor bridge from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_language_runtime_editor_bridge_init(UmiLanguageRuntimeEditorBridge*b,UmiLanguageService*l,UmiEditorSession*e){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!b||!l||!e)return UMI_STATUS_INVALID_ARGUMENT;b->language=l;b->editor=e;b->revision=1;return UMI_STATUS_OK;}
/*
 * Provide the language runtime editor bridge sync document operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_runtime_editor_bridge_sync_document(UmiLanguageRuntimeEditorBridge*b,const char*d,const char*title,uint64_t bytes){UmiLanguageDocumentSnapshot s;UmiEditorDocumentSnapshot t={0};UmiStatus q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!b||!d||!title)return UMI_STATUS_INVALID_ARGUMENT;q=umi_language_document_registry_find(umi_language_service_document(b->language),d,&s);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;t.struct_size=sizeof(t);t.api_version=1;snprintf(t.id,sizeof(t.id),"%s",s.id);snprintf(t.uri,sizeof(t.uri),"%s",s.uri);snprintf(t.language_id,sizeof(t.language_id),"%s",s.language_id);snprintf(t.title,sizeof(t.title),"%s",title);t.version=s.version;t.byte_count=bytes;t.line_count=s.line_count;t.dirty=s.dirty;t.read_only=0;t.revision=s.revision;q=umi_editor_document_registry_upsert(umi_editor_session_document(b->editor),&t);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q==UMI_STATUS_OK)b->revision++;return q;}
/* Provide the cc operation used by this module and its client applications. */
static void cc(UmiEditorCompletionRegistry*r,const char*d){size_t i=umi_editor_completion_registry_count(r);/* Continue only while work remains available; the loop body advances the state on each pass. */ while(i--){UmiEditorCompletionSnapshot x;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_completion_registry_at(r,i,&x)==UMI_STATUS_OK&&same(x.document_id,d))(void)umi_editor_completion_registry_remove(r,x.id);}}
/*
 * Provide the language runtime editor bridge sync completion operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_runtime_editor_bridge_sync_completion(UmiLanguageRuntimeEditorBridge*b,const char*d){UmiLanguageCompletionRegistry*s;UmiEditorCompletionRegistry*t;size_t i;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!b||!d)return UMI_STATUS_INVALID_ARGUMENT;s=umi_language_service_completion(b->language);t=umi_editor_session_completion(b->editor);cc(t,d);/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<umi_language_completion_registry_count(s);i++){UmiLanguageCompletionSnapshot a;UmiEditorCompletionSnapshot x={0};UmiStatus q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_language_completion_registry_at(s,i,&a)!=UMI_STATUS_OK||!same(a.document_id,d))continue;x.struct_size=sizeof(x);x.api_version=1;snprintf(x.id,sizeof(x.id),"%s",a.id);snprintf(x.document_id,sizeof(x.document_id),"%s",a.document_id);snprintf(x.label,sizeof(x.label),"%s",a.label);snprintf(x.detail,sizeof(x.detail),"%s",a.detail);snprintf(x.insert_text,sizeof(x.insert_text),"%s",a.insert_text);snprintf(x.kind,sizeof(x.kind),"%s",a.kind);snprintf(x.sort_text,sizeof(x.sort_text),"%s",a.sort_text);snprintf(x.filter_text,sizeof(x.filter_text),"%s",a.label);x.deprecated=0;x.revision=a.revision;q=umi_editor_completion_registry_upsert(t,&x);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;}b->revision++;return UMI_STATUS_OK;}
/* Provide the cd operation used by this module and its client applications. */
static void cd(UmiEditorDiagnosticRegistry*r,const char*d){size_t i=umi_editor_diagnostic_registry_count(r);/* Continue only while work remains available; the loop body advances the state on each pass. */ while(i--){UmiEditorDiagnosticSnapshot x;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_diagnostic_registry_at(r,i,&x)==UMI_STATUS_OK&&same(x.document_id,d))(void)umi_editor_diagnostic_registry_remove(r,x.id);}}
/*
 * Provide the language runtime editor bridge sync diagnostics operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_runtime_editor_bridge_sync_diagnostics(UmiLanguageRuntimeEditorBridge*b,const char*d){UmiLanguageDiagnosticRegistry*s;UmiEditorDiagnosticRegistry*t;size_t i;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!b||!d)return UMI_STATUS_INVALID_ARGUMENT;s=umi_language_service_diagnostic(b->language);t=umi_editor_session_diagnostic(b->editor);cd(t,d);/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<umi_language_diagnostic_registry_count(s);i++){UmiLanguageDiagnosticSnapshot a;UmiEditorDiagnosticSnapshot x={0};UmiStatus q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_language_diagnostic_registry_at(s,i,&a)!=UMI_STATUS_OK||!same(a.document_id,d))continue;x.struct_size=sizeof(x);x.api_version=1;snprintf(x.id,sizeof(x.id),"%s",a.id);snprintf(x.document_id,sizeof(x.document_id),"%s",a.document_id);snprintf(x.source,sizeof(x.source),"%s",a.source);snprintf(x.code,sizeof(x.code),"%s",a.code);snprintf(x.message,sizeof(x.message),"%s",a.message);x.severity=a.severity;x.line=a.line;x.column=a.column;x.end_line=a.end_line;x.end_column=a.end_column;x.revision=a.revision;q=umi_editor_diagnostic_registry_upsert(t,&x);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;}b->revision++;return UMI_STATUS_OK;}
/* Provide the cs operation used by this module and its client applications. */
static void cs(UmiEditorSymbolRegistry*r,const char*d){size_t i=umi_editor_symbol_registry_count(r);/* Continue only while work remains available; the loop body advances the state on each pass. */ while(i--){UmiEditorSymbolSnapshot x;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_symbol_registry_at(r,i,&x)==UMI_STATUS_OK&&same(x.document_id,d))(void)umi_editor_symbol_registry_remove(r,x.id);}}
/*
 * Provide the language runtime editor bridge sync symbols operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_runtime_editor_bridge_sync_symbols(UmiLanguageRuntimeEditorBridge*b,const char*d){UmiLanguageSymbolRegistry*s;UmiEditorSymbolRegistry*t;size_t i;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!b||!d)return UMI_STATUS_INVALID_ARGUMENT;s=umi_language_service_symbol(b->language);t=umi_editor_session_symbol(b->editor);cs(t,d);/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<umi_language_symbol_registry_count(s);i++){UmiLanguageSymbolSnapshot a;UmiEditorSymbolSnapshot x={0};UmiStatus q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_language_symbol_registry_at(s,i,&a)!=UMI_STATUS_OK||!same(a.document_id,d))continue;x.struct_size=sizeof(x);x.api_version=1;snprintf(x.id,sizeof(x.id),"%s",a.id);snprintf(x.document_id,sizeof(x.document_id),"%s",a.document_id);snprintf(x.name,sizeof(x.name),"%s",a.name);snprintf(x.kind,sizeof(x.kind),"%s",a.kind);snprintf(x.detail,sizeof(x.detail),"%s",a.container);x.line=a.line;x.column=a.column;x.end_line=a.end_line;x.end_column=a.end_column;x.revision=a.revision;q=umi_editor_symbol_registry_upsert(t,&x);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;}b->revision++;return UMI_STATUS_OK;}
/* Provide the ca operation used by this module and its client applications. */
static void ca(UmiEditorCodeActionRegistry*r,const char*d){size_t i=umi_editor_code_action_registry_count(r);/* Continue only while work remains available; the loop body advances the state on each pass. */ while(i--){UmiEditorCodeActionSnapshot x;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_code_action_registry_at(r,i,&x)==UMI_STATUS_OK&&same(x.document_id,d))(void)umi_editor_code_action_registry_remove(r,x.id);}}
/*
 * Provide the language runtime editor bridge sync code actions operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_runtime_editor_bridge_sync_code_actions(UmiLanguageRuntimeEditorBridge*b,const char*d){UmiLanguageCodeActionRegistry*s;UmiEditorCodeActionRegistry*t;size_t i;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!b||!d)return UMI_STATUS_INVALID_ARGUMENT;s=umi_language_service_code_action(b->language);t=umi_editor_session_code_action(b->editor);ca(t,d);/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<umi_language_code_action_registry_count(s);i++){UmiLanguageCodeActionSnapshot a;UmiEditorCodeActionSnapshot x={0};UmiStatus q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_language_code_action_registry_at(s,i,&a)!=UMI_STATUS_OK||!same(a.document_id,d))continue;x.struct_size=sizeof(x);x.api_version=1;snprintf(x.id,sizeof(x.id),"%s",a.id);snprintf(x.document_id,sizeof(x.document_id),"%s",a.document_id);snprintf(x.title,sizeof(x.title),"%s",a.title);snprintf(x.kind,sizeof(x.kind),"%s",a.kind);snprintf(x.command_id,sizeof(x.command_id),"%s",a.command_id);snprintf(x.argument,sizeof(x.argument),"%s",a.argument);x.preferred=a.preferred;x.enabled=1;x.revision=a.revision;q=umi_editor_code_action_registry_upsert(t,&x);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;}b->revision++;return UMI_STATUS_OK;}
