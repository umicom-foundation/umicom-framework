/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/request_context.h
 *
 * PURPOSE:
 *   Retain asynchronous editor/symbol context until an LSP response returns.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_REQUEST_CONTEXT_H
#define UMICOM_LANGUAGE_RUNTIME_REQUEST_CONTEXT_H
#include "umicom/language_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the language runtime request context data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageRuntimeRequestContext{uint64_t request_id;char method[UMI_LANGUAGE_RUNTIME_NAME_CAPACITY];char document_id[UMI_LANGUAGE_RUNTIME_ID_CAPACITY];char uri[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY];char symbol_id[UMI_LANGUAGE_RUNTIME_ID_CAPACITY];char old_name[UMI_LANGUAGE_RUNTIME_NAME_CAPACITY];char argument[UMI_LANGUAGE_RUNTIME_NAME_CAPACITY];uint32_t line,column;uint64_t sequence;int active;}UmiLanguageRuntimeRequestContext;
/**
 * Represent the language runtime request context table data shared with callers of this
 * public contract.
 */
typedef struct UmiLanguageRuntimeRequestContextTable{UmiLanguageRuntimeRequestContext items[UMI_LANGUAGE_RUNTIME_MAX_PENDING_REQUESTS];size_t active_count;uint64_t sequence;}UmiLanguageRuntimeRequestContextTable;
/**
 * Initialise language runtime request context from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_runtime_request_context_init(UmiLanguageRuntimeRequestContextTable*t);
/**
 * Add language runtime request context only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_language_runtime_request_context_add(UmiLanguageRuntimeRequestContextTable*t,const UmiLanguageRuntimeRequestContext*c);
/**
 * Provide the language runtime request context take operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_request_context_take(UmiLanguageRuntimeRequestContextTable*t,uint64_t id,UmiLanguageRuntimeRequestContext*out);
#ifdef __cplusplus
}
#endif
#endif
