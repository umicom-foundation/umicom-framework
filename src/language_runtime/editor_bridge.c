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
/* The previous bridge cleared visible rows before converting all replacements.
 * The staged implementation below keeps the editor's last complete result when
 * conversion, capacity or revision checks fail. Retain the original bridge for
 * engineering review of its field mappings and previous publication behavior. */
#if 0
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
#endif

#include "publication_internal.h"

/* Language and Editor own separate registries. Copy a complete document into
 * private storage, then publish to Editor once; no application duplicates this
 * ownership or rollback policy. Callers serialize access on the owner thread. */
static UmiStatus editor_publication_start(UmiLanguageRuntimeEditorBridge *bridge,
    const char *document_id)
{
    if (bridge == NULL || bridge->language == NULL || bridge->editor == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (bridge->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    return umi_language_publication_document(document_id,
        sizeof(((UmiEditorDiagnosticSnapshot *)0)->document_id));
}

UmiStatus umi_language_runtime_editor_bridge_init(UmiLanguageRuntimeEditorBridge *b,
    UmiLanguageService *language, UmiEditorSession *editor)
{
    if (b == NULL || language == NULL || editor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    b->language = language; b->editor = editor; b->revision = 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_language_runtime_editor_bridge_sync_document(UmiLanguageRuntimeEditorBridge *b,
    const char *d, const char *title, uint64_t bytes)
{
    UmiLanguageDocumentSnapshot source;
    UmiEditorDocumentSnapshot item = {0};
    UmiStatus status = editor_publication_start(b, d);
    if (status != UMI_STATUS_OK) return status;
    if (title == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_language_document_registry_find(umi_language_service_document(b->language), d, &source);
    if (status != UMI_STATUS_OK) return status;
    item.struct_size = (uint32_t)sizeof(item); item.api_version = 1U;
    status = umi_language_publication_copy(item.id, sizeof(item.id), source.id, sizeof(source.id));
    if (status != UMI_STATUS_OK) return status;
    status = umi_language_publication_copy(item.uri, sizeof(item.uri), source.uri, sizeof(source.uri));
    if (status != UMI_STATUS_OK) return status;
    status = umi_language_publication_copy(item.language_id, sizeof(item.language_id), source.language_id, sizeof(source.language_id));
    if (status != UMI_STATUS_OK) return status;
    status = umi_language_publication_copy(item.title, sizeof(item.title), title, sizeof(item.title));
    if (status != UMI_STATUS_OK) return status;
    item.version = source.version; item.byte_count = bytes;
    item.line_count = source.line_count; item.dirty = source.dirty;
    item.read_only = 0; item.revision = source.revision;
    status = umi_editor_document_registry_upsert(umi_editor_session_document(b->editor), &item);
    if (status == UMI_STATUS_OK) ++b->revision;
    return status;
}

UmiStatus umi_language_runtime_editor_bridge_sync_completion(UmiLanguageRuntimeEditorBridge *b, const char *d)
{
    UmiStatus status = editor_publication_start(b, d);
    UmiEditorCompletionSnapshot *items = NULL;
    size_t count = 0U, written = 0U;
    if (status != UMI_STATUS_OK) return status;
    UmiLanguageCompletionRegistry *source = umi_language_service_completion(b->language);
    UmiEditorCompletionRegistry *target = umi_editor_session_completion(b->editor);
    const uint64_t source_revision = umi_language_completion_registry_revision(source);
    const uint64_t expected = umi_editor_completion_registry_revision(target);
    const size_t total = umi_language_completion_registry_count(source);
    for (size_t i = 0U; i < total; ++i) {
        UmiLanguageCompletionSnapshot a;
        status = umi_language_completion_registry_at(source, i, &a);
        if (status != UMI_STATUS_OK) return status;
        status = umi_language_completion_snapshot_validate(&a, NULL);
        if (status != UMI_STATUS_OK) return status;
        if (strcmp(a.document_id, d) == 0) ++count;
    }
    if (count > UMI_EDITOR_COMPLETION_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (count != 0U) {
        items = calloc(count, sizeof(*items));
        if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    }
    for (size_t i = 0U; i < total; ++i) {
        UmiLanguageCompletionSnapshot a;
        status = umi_language_completion_registry_at(source, i, &a);
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_completion_snapshot_validate(&a, NULL);
        if (status != UMI_STATUS_OK) goto done;
        if (strcmp(a.document_id, d) != 0) continue;
        if (written == count) { status = UMI_STATUS_INVALID_STATE; goto done; }
        UmiEditorCompletionSnapshot *x = &items[written++];
        x->struct_size = (uint32_t)sizeof(*x); x->api_version = 1U;
        status = umi_language_publication_copy(x->id, sizeof(x->id), a.id, sizeof(a.id));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->document_id, sizeof(x->document_id), a.document_id, sizeof(a.document_id));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->label, sizeof(x->label), a.label, sizeof(a.label));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->detail, sizeof(x->detail), a.detail, sizeof(a.detail));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->insert_text, sizeof(x->insert_text), a.insert_text, sizeof(a.insert_text));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->kind, sizeof(x->kind), a.kind, sizeof(a.kind));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->sort_text, sizeof(x->sort_text), a.sort_text, sizeof(a.sort_text));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->filter_text, sizeof(x->filter_text), a.label, sizeof(a.label));
        if (status != UMI_STATUS_OK) goto done;
        x->deprecated = 0;
        x->revision = a.revision;
    }
    if (written != count || umi_language_completion_registry_revision(source) != source_revision) {
        status = UMI_STATUS_INVALID_STATE; goto done;
    }
    status = umi_editor_completion_registry_replace_document(target, d, expected, items, count, NULL);
    if (status == UMI_STATUS_OK) ++b->revision;
done:
    free(items);
    return status;
}

UmiStatus umi_language_runtime_editor_bridge_sync_diagnostics(UmiLanguageRuntimeEditorBridge *b, const char *d)
{
    UmiStatus status = editor_publication_start(b, d);
    UmiEditorDiagnosticSnapshot *items = NULL;
    size_t count = 0U, written = 0U;
    if (status != UMI_STATUS_OK) return status;
    UmiLanguageDiagnosticRegistry *source = umi_language_service_diagnostic(b->language);
    UmiEditorDiagnosticRegistry *target = umi_editor_session_diagnostic(b->editor);
    const uint64_t source_revision = umi_language_diagnostic_registry_revision(source);
    const uint64_t expected = umi_editor_diagnostic_registry_revision(target);
    const size_t total = umi_language_diagnostic_registry_count(source);
    for (size_t i = 0U; i < total; ++i) {
        UmiLanguageDiagnosticSnapshot a;
        status = umi_language_diagnostic_registry_at(source, i, &a);
        if (status != UMI_STATUS_OK) return status;
        status = umi_language_diagnostic_snapshot_validate(&a, NULL);
        if (status != UMI_STATUS_OK) return status;
        if (strcmp(a.document_id, d) == 0) ++count;
    }
    if (count > UMI_EDITOR_DIAGNOSTIC_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (count != 0U) {
        items = calloc(count, sizeof(*items));
        if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    }
    for (size_t i = 0U; i < total; ++i) {
        UmiLanguageDiagnosticSnapshot a;
        status = umi_language_diagnostic_registry_at(source, i, &a);
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_diagnostic_snapshot_validate(&a, NULL);
        if (status != UMI_STATUS_OK) goto done;
        if (strcmp(a.document_id, d) != 0) continue;
        if (written == count) { status = UMI_STATUS_INVALID_STATE; goto done; }
        UmiEditorDiagnosticSnapshot *x = &items[written++];
        x->struct_size = (uint32_t)sizeof(*x); x->api_version = 1U;
        status = umi_language_publication_copy(x->id, sizeof(x->id), a.id, sizeof(a.id));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->document_id, sizeof(x->document_id), a.document_id, sizeof(a.document_id));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->source, sizeof(x->source), a.source, sizeof(a.source));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->code, sizeof(x->code), a.code, sizeof(a.code));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->message, sizeof(x->message), a.message, sizeof(a.message));
        if (status != UMI_STATUS_OK) goto done;
        x->severity = a.severity;
        x->line = a.line;
        x->column = a.column;
        x->end_line = a.end_line;
        x->end_column = a.end_column;
        x->revision = a.revision;
    }
    if (written != count || umi_language_diagnostic_registry_revision(source) != source_revision) {
        status = UMI_STATUS_INVALID_STATE; goto done;
    }
    status = umi_editor_diagnostic_registry_replace_document(target, d, expected, items, count, NULL);
    if (status == UMI_STATUS_OK) ++b->revision;
done:
    free(items);
    return status;
}

UmiStatus umi_language_runtime_editor_bridge_sync_symbols(UmiLanguageRuntimeEditorBridge *b, const char *d)
{
    UmiStatus status = editor_publication_start(b, d);
    UmiEditorSymbolSnapshot *items = NULL;
    size_t count = 0U, written = 0U;
    if (status != UMI_STATUS_OK) return status;
    UmiLanguageSymbolRegistry *source = umi_language_service_symbol(b->language);
    UmiEditorSymbolRegistry *target = umi_editor_session_symbol(b->editor);
    const uint64_t source_revision = umi_language_symbol_registry_revision(source);
    const uint64_t expected = umi_editor_symbol_registry_revision(target);
    const size_t total = umi_language_symbol_registry_count(source);
    for (size_t i = 0U; i < total; ++i) {
        UmiLanguageSymbolSnapshot a;
        status = umi_language_symbol_registry_at(source, i, &a);
        if (status != UMI_STATUS_OK) return status;
        status = umi_language_symbol_snapshot_validate(&a, NULL);
        if (status != UMI_STATUS_OK) return status;
        if (strcmp(a.document_id, d) == 0) ++count;
    }
    if (count > UMI_EDITOR_SYMBOL_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (count != 0U) {
        items = calloc(count, sizeof(*items));
        if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    }
    for (size_t i = 0U; i < total; ++i) {
        UmiLanguageSymbolSnapshot a;
        status = umi_language_symbol_registry_at(source, i, &a);
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_symbol_snapshot_validate(&a, NULL);
        if (status != UMI_STATUS_OK) goto done;
        if (strcmp(a.document_id, d) != 0) continue;
        if (written == count) { status = UMI_STATUS_INVALID_STATE; goto done; }
        UmiEditorSymbolSnapshot *x = &items[written++];
        x->struct_size = (uint32_t)sizeof(*x); x->api_version = 1U;
        status = umi_language_publication_copy(x->id, sizeof(x->id), a.id, sizeof(a.id));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->document_id, sizeof(x->document_id), a.document_id, sizeof(a.document_id));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->name, sizeof(x->name), a.name, sizeof(a.name));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->kind, sizeof(x->kind), a.kind, sizeof(a.kind));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->detail, sizeof(x->detail), a.container, sizeof(a.container));
        if (status != UMI_STATUS_OK) goto done;
        x->line = a.line;
        x->column = a.column;
        x->end_line = a.end_line;
        x->end_column = a.end_column;
        x->revision = a.revision;
    }
    if (written != count || umi_language_symbol_registry_revision(source) != source_revision) {
        status = UMI_STATUS_INVALID_STATE; goto done;
    }
    status = umi_editor_symbol_registry_replace_document(target, d, expected, items, count, NULL);
    if (status == UMI_STATUS_OK) ++b->revision;
done:
    free(items);
    return status;
}

UmiStatus umi_language_runtime_editor_bridge_sync_code_actions(UmiLanguageRuntimeEditorBridge *b, const char *d)
{
    UmiStatus status = editor_publication_start(b, d);
    UmiEditorCodeActionSnapshot *items = NULL;
    size_t count = 0U, written = 0U;
    if (status != UMI_STATUS_OK) return status;
    UmiLanguageCodeActionRegistry *source = umi_language_service_code_action(b->language);
    UmiEditorCodeActionRegistry *target = umi_editor_session_code_action(b->editor);
    const uint64_t source_revision = umi_language_code_action_registry_revision(source);
    const uint64_t expected = umi_editor_code_action_registry_revision(target);
    const size_t total = umi_language_code_action_registry_count(source);
    for (size_t i = 0U; i < total; ++i) {
        UmiLanguageCodeActionSnapshot a;
        status = umi_language_code_action_registry_at(source, i, &a);
        if (status != UMI_STATUS_OK) return status;
        status = umi_language_code_action_snapshot_validate(&a, NULL);
        if (status != UMI_STATUS_OK) return status;
        if (strcmp(a.document_id, d) == 0) ++count;
    }
    if (count > UMI_EDITOR_CODE_ACTION_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (count != 0U) {
        items = calloc(count, sizeof(*items));
        if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    }
    for (size_t i = 0U; i < total; ++i) {
        UmiLanguageCodeActionSnapshot a;
        status = umi_language_code_action_registry_at(source, i, &a);
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_code_action_snapshot_validate(&a, NULL);
        if (status != UMI_STATUS_OK) goto done;
        if (strcmp(a.document_id, d) != 0) continue;
        if (written == count) { status = UMI_STATUS_INVALID_STATE; goto done; }
        UmiEditorCodeActionSnapshot *x = &items[written++];
        x->struct_size = (uint32_t)sizeof(*x); x->api_version = 1U;
        status = umi_language_publication_copy(x->id, sizeof(x->id), a.id, sizeof(a.id));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->document_id, sizeof(x->document_id), a.document_id, sizeof(a.document_id));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->title, sizeof(x->title), a.title, sizeof(a.title));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->kind, sizeof(x->kind), a.kind, sizeof(a.kind));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->command_id, sizeof(x->command_id), a.command_id, sizeof(a.command_id));
        if (status != UMI_STATUS_OK) goto done;
        status = umi_language_publication_copy(x->argument, sizeof(x->argument), a.argument, sizeof(a.argument));
        if (status != UMI_STATUS_OK) goto done;
        x->preferred = a.preferred; x->enabled = 1;
        x->revision = a.revision;
    }
    if (written != count || umi_language_code_action_registry_revision(source) != source_revision) {
        status = UMI_STATUS_INVALID_STATE; goto done;
    }
    status = umi_editor_code_action_registry_replace_document(target, d, expected, items, count, NULL);
    if (status == UMI_STATUS_OK) ++b->revision;
done:
    free(items);
    return status;
}
