/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/results.h
 *
 * PURPOSE:
 *   Define bounded decoded LSP results before projection into existing Language/Editor models.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_RESULTS_H
#define UMICOM_LANGUAGE_RUNTIME_RESULTS_H
#include "umicom/language_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_RUNTIME_COMPLETION_CAPACITY 128U
#define UMI_LANGUAGE_RUNTIME_LOCATION_CAPACITY 256U
#define UMI_LANGUAGE_RUNTIME_SYMBOL_CAPACITY 256U
#define UMI_LANGUAGE_RUNTIME_DIAGNOSTIC_CAPACITY 256U
#define UMI_LANGUAGE_RUNTIME_CODE_ACTION_CAPACITY 128U
#define UMI_LANGUAGE_RUNTIME_HINT_CAPACITY 256U
#define UMI_LANGUAGE_RUNTIME_FOLD_CAPACITY 256U
#define UMI_LANGUAGE_RUNTIME_SEMANTIC_DATA_CAPACITY 4096U
typedef struct UmiLanguageRuntimeCompletionItem{char label[256],detail[512],insert_text[1024],sort_text[256];int kind;}UmiLanguageRuntimeCompletionItem;
typedef struct UmiLanguageRuntimeCompletionResult{UmiLanguageRuntimeCompletionItem items[UMI_LANGUAGE_RUNTIME_COMPLETION_CAPACITY];size_t count;int incomplete;}UmiLanguageRuntimeCompletionResult;
typedef struct UmiLanguageRuntimeHoverResult{char contents[4096];UmiLanguageRuntimeRange range;int has_range;}UmiLanguageRuntimeHoverResult;
typedef struct UmiLanguageRuntimeSignatureResult{char label[1024],documentation[2048];uint32_t active_signature,active_parameter;int available;}UmiLanguageRuntimeSignatureResult;
typedef struct UmiLanguageRuntimeLocationList{UmiLanguageRuntimeLocation items[UMI_LANGUAGE_RUNTIME_LOCATION_CAPACITY];size_t count;}UmiLanguageRuntimeLocationList;
typedef struct UmiLanguageRuntimeSymbolItem{char name[256],container[256],uri[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY];int kind;UmiLanguageRuntimeRange range;}UmiLanguageRuntimeSymbolItem;
typedef struct UmiLanguageRuntimeSymbolList{UmiLanguageRuntimeSymbolItem items[UMI_LANGUAGE_RUNTIME_SYMBOL_CAPACITY];size_t count;}UmiLanguageRuntimeSymbolList;
typedef struct UmiLanguageRuntimeDiagnosticItem{UmiLanguageRuntimeRange range;int severity;char code[128],source[128],message[1024];}UmiLanguageRuntimeDiagnosticItem;
typedef struct UmiLanguageRuntimeDiagnosticList{char uri[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY];uint64_t version;UmiLanguageRuntimeDiagnosticItem items[UMI_LANGUAGE_RUNTIME_DIAGNOSTIC_CAPACITY];size_t count;}UmiLanguageRuntimeDiagnosticList;
typedef struct UmiLanguageRuntimeCodeActionItem{char title[512],kind[128],command[256];int preferred;}UmiLanguageRuntimeCodeActionItem;
typedef struct UmiLanguageRuntimeCodeActionList{UmiLanguageRuntimeCodeActionItem items[UMI_LANGUAGE_RUNTIME_CODE_ACTION_CAPACITY];size_t count;}UmiLanguageRuntimeCodeActionList;
typedef struct UmiLanguageRuntimeTextEditList{UmiLanguageRuntimeTextEdit edits[UMI_LANGUAGE_RUNTIME_MAX_EDITS];size_t count;}UmiLanguageRuntimeTextEditList;
typedef struct UmiLanguageRuntimeWorkspaceEditItem{char uri[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY];UmiLanguageRuntimeTextEdit edit;}UmiLanguageRuntimeWorkspaceEditItem;
typedef struct UmiLanguageRuntimeWorkspaceEdit{UmiLanguageRuntimeWorkspaceEditItem items[UMI_LANGUAGE_RUNTIME_MAX_EDITS];size_t count;}UmiLanguageRuntimeWorkspaceEdit;
typedef struct UmiLanguageRuntimeSemanticTokens{uint32_t data[UMI_LANGUAGE_RUNTIME_SEMANTIC_DATA_CAPACITY];size_t count;}UmiLanguageRuntimeSemanticTokens;
typedef struct UmiLanguageRuntimeInlayHint{UmiLanguageRuntimePosition position;char label[512];int kind,padding_left,padding_right;}UmiLanguageRuntimeInlayHint;
typedef struct UmiLanguageRuntimeInlayHintList{UmiLanguageRuntimeInlayHint items[UMI_LANGUAGE_RUNTIME_HINT_CAPACITY];size_t count;}UmiLanguageRuntimeInlayHintList;
typedef struct UmiLanguageRuntimeFoldingRange{uint32_t start_line,start_character,end_line,end_character;char kind[64];}UmiLanguageRuntimeFoldingRange;
typedef struct UmiLanguageRuntimeFoldingRangeList{UmiLanguageRuntimeFoldingRange items[UMI_LANGUAGE_RUNTIME_FOLD_CAPACITY];size_t count;}UmiLanguageRuntimeFoldingRangeList;
typedef struct UmiLanguageRuntimeInitializeResult{int text_document_sync,completion,hover,signature_help,definition,references,document_symbols,workspace_symbols,code_actions,formatting,rename,semantic_tokens,inlay_hints,folding_ranges;}UmiLanguageRuntimeInitializeResult;
#ifdef __cplusplus
}
#endif
#endif
