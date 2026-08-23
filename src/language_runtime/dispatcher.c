/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/dispatcher.c
 *
 * PURPOSE:
 *   Implement method-aware response and publishDiagnostics dispatch into Language/Editor models.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/dispatcher.h"
#include <stdio.h>
#include <string.h>
void umi_language_runtime_dispatch_state_init(UmiLanguageRuntimeDispatchState*s){if(s){memset(s,0,sizeof(*s));s->revision=1;}}
static const char*doc(const UmiLanguageRuntimeRequestContext*c){return c?c->document_id:"";}static const char*sym(const UmiLanguageRuntimeRequestContext*c){return c&&c->symbol_id[0]?c->symbol_id:"selection";}
UmiStatus umi_language_runtime_dispatch(UmiLanguageRuntimeServiceBridge*l,UmiLanguageRuntimeEditorBridge*e,UmiLanguageRuntimeDocumentSync*d,const UmiLanguageRuntimeEnvelope*env,const UmiLanguageRuntimeRequestContext*c,UmiLanguageRuntimeDispatchState*st,int*handled){UmiStatus q=UMI_STATUS_OK;if(!l||!d||!env||!st||!handled)return UMI_STATUS_INVALID_ARGUMENT;*handled=1;if(env->kind==UMI_LANGUAGE_RUNTIME_MESSAGE_ERROR){st->errors_received++;snprintf(st->last_error,sizeof(st->last_error),"%s",env->error_message);st->revision++;return UMI_STATUS_OK;}if(strcmp(env->method,"textDocument/completion")==0){UmiLanguageRuntimeCompletionResult x;q=umi_language_runtime_decode_completion(env->json,&x);if(q==UMI_STATUS_OK)q=umi_language_runtime_publish_completion(l,doc(c),c?c->line:0,c?c->column:0,&x);if(q==UMI_STATUS_OK&&e)q=umi_language_runtime_editor_bridge_sync_completion(e,doc(c));}
else if(strcmp(env->method,"textDocument/hover")==0){UmiLanguageRuntimeHoverResult x;q=umi_language_runtime_decode_hover(env->json,&x);if(q==UMI_STATUS_OK)q=umi_language_runtime_publish_hover(l,doc(c),c?c->line:0,c?c->column:0,&x);}
else if(strcmp(env->method,"textDocument/signatureHelp")==0){UmiLanguageRuntimeSignatureResult x;q=umi_language_runtime_decode_signature(env->json,&x);if(q==UMI_STATUS_OK)q=umi_language_runtime_publish_signature(l,doc(c),c?c->line:0,c?c->column:0,&x);}
else if(strcmp(env->method,"textDocument/definition")==0||strcmp(env->method,"textDocument/references")==0){UmiLanguageRuntimeLocationList x;q=umi_language_runtime_decode_locations(env->json,&x);if(q==UMI_STATUS_OK)q=umi_language_runtime_publish_locations(l,doc(c),sym(c),strcmp(env->method,"textDocument/definition")==0,&x);}
else if(strcmp(env->method,"textDocument/documentSymbol")==0||strcmp(env->method,"workspace/symbol")==0){UmiLanguageRuntimeSymbolList x;const char*dd=doc(c);q=umi_language_runtime_decode_symbols(env->json,c?c->uri:"",&x);if(q==UMI_STATUS_OK)q=umi_language_runtime_publish_symbols(l,*dd?dd:"workspace",&x);if(q==UMI_STATUS_OK&&e&&*dd)q=umi_language_runtime_editor_bridge_sync_symbols(e,dd);}
else if(strcmp(env->method,"textDocument/publishDiagnostics")==0){UmiLanguageRuntimeDiagnosticList x;UmiLanguageRuntimeDocumentState ds;q=umi_language_runtime_decode_diagnostics(env->json,&x);if(q==UMI_STATUS_OK){q=umi_language_runtime_document_find_by_uri(d,x.uri,&ds);if(q==UMI_STATUS_NOT_FOUND)q=UMI_STATUS_OK;else if(q==UMI_STATUS_OK){q=umi_language_runtime_publish_diagnostics(l,ds.document_id,&x);if(q==UMI_STATUS_OK&&e)q=umi_language_runtime_editor_bridge_sync_diagnostics(e,ds.document_id);}}}
else if(strcmp(env->method,"textDocument/codeAction")==0){UmiLanguageRuntimeCodeActionList x;q=umi_language_runtime_decode_code_actions(env->json,&x);if(q==UMI_STATUS_OK)q=umi_language_runtime_publish_code_actions(l,doc(c),&x);if(q==UMI_STATUS_OK&&e)q=umi_language_runtime_editor_bridge_sync_code_actions(e,doc(c));}
else if(strcmp(env->method,"textDocument/formatting")==0){q=umi_language_runtime_decode_text_edits(env->json,&st->last_text_edits);if(q==UMI_STATUS_OK)q=umi_language_runtime_publish_formatting_available(l,doc(c),"lsp",4,1);}
else if(strcmp(env->method,"textDocument/rename")==0){q=umi_language_runtime_decode_workspace_edit(env->json,&st->last_workspace_edit);if(q==UMI_STATUS_OK)q=umi_language_runtime_publish_rename(l,doc(c),sym(c),c?c->old_name:"",c?c->argument:"",&st->last_workspace_edit);}
else if(strcmp(env->method,"textDocument/semanticTokens/full")==0){UmiLanguageRuntimeSemanticTokens x;q=umi_language_runtime_decode_semantic_tokens(env->json,&x);if(q==UMI_STATUS_OK)q=umi_language_runtime_publish_semantic_tokens(l,doc(c),&x);}
else if(strcmp(env->method,"textDocument/inlayHint")==0){UmiLanguageRuntimeInlayHintList x;q=umi_language_runtime_decode_inlay_hints(env->json,&x);if(q==UMI_STATUS_OK)q=umi_language_runtime_publish_inlay_hints(l,doc(c),&x);}
else if(strcmp(env->method,"textDocument/foldingRange")==0){UmiLanguageRuntimeFoldingRangeList x;q=umi_language_runtime_decode_folding_ranges(env->json,&x);if(q==UMI_STATUS_OK)q=umi_language_runtime_publish_folding_ranges(l,doc(c),&x);}
else if (strcmp(env->method, "initialize") == 0) {
    q = umi_language_runtime_decode_initialize(env->json, &st->last_initialize);
} else {
    *handled = 0;
    return UMI_STATUS_OK;
}

if (q == UMI_STATUS_OK) {
    st->messages_dispatched++;
    st->revision++;
}
return q;
}
