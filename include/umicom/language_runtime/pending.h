/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/pending.h
 *
 * PURPOSE:
 *   Track outstanding JSON-RPC requests by ID, method and document.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_PENDING_H
#define UMICOM_LANGUAGE_RUNTIME_PENDING_H
#include "umicom/language_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the language runtime pending request data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageRuntimePendingRequest{uint64_t request_id;char method[UMI_LANGUAGE_RUNTIME_NAME_CAPACITY];char document_id[UMI_LANGUAGE_RUNTIME_ID_CAPACITY];uint64_t sequence;int active;}UmiLanguageRuntimePendingRequest;
/**
 * Represent the language runtime pending table data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageRuntimePendingTable{UmiLanguageRuntimePendingRequest items[UMI_LANGUAGE_RUNTIME_MAX_PENDING_REQUESTS];uint64_t sequence;size_t active_count;}UmiLanguageRuntimePendingTable;
/**
 * Initialise language runtime pending from caller-provided values so later operations
 * receive a known state.
 */
void umi_language_runtime_pending_init(UmiLanguageRuntimePendingTable*t);
/**
 * Add language runtime pending only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_language_runtime_pending_add(UmiLanguageRuntimePendingTable*t,uint64_t id,const char*m,const char*d);
/**
 * Provide the language runtime pending take operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_pending_take(UmiLanguageRuntimePendingTable*t,uint64_t id,UmiLanguageRuntimePendingRequest*out);
/**
 * Find language runtime pending while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_language_runtime_pending_find(const UmiLanguageRuntimePendingTable*t,uint64_t id,UmiLanguageRuntimePendingRequest*out);
#ifdef __cplusplus
}
#endif
#endif
