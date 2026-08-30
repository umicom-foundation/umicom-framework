/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/dispatcher.h
 *
 * PURPOSE:
 *   Route correlated LSP responses/notifications through decoders into existing Framework models.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DISPATCHER_H
#define UMICOM_LANGUAGE_RUNTIME_DISPATCHER_H
#include "umicom/language_runtime/document_sync.h"
#include "umicom/language_runtime/editor_bridge.h"
#include "umicom/language_runtime/request_context.h"
#include "umicom/language_runtime/service_bridge.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiLanguageRuntimeDispatchState{UmiLanguageRuntimeTextEditList last_text_edits;UmiLanguageRuntimeWorkspaceEdit last_workspace_edit;UmiLanguageRuntimeInitializeResult last_initialize;uint64_t messages_dispatched,errors_received,revision;char last_error[UMI_LANGUAGE_RUNTIME_TEXT_CAPACITY];}UmiLanguageRuntimeDispatchState;
void umi_language_runtime_dispatch_state_init(UmiLanguageRuntimeDispatchState*s);
UmiStatus umi_language_runtime_dispatch(UmiLanguageRuntimeServiceBridge*l,UmiLanguageRuntimeEditorBridge*e,UmiLanguageRuntimeDocumentSync*d,const UmiLanguageRuntimeEnvelope*env,const UmiLanguageRuntimeRequestContext*ctx,UmiLanguageRuntimeDispatchState*state,int*handled);
#ifdef __cplusplus
}
#endif
#endif
