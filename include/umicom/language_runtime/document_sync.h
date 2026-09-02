/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/document_sync.h
 *
 * PURPOSE:
 *   Keep existing LanguageDocument registry and real LSP didOpen/change/save/close lifecycle synchronized.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DOCUMENT_SYNC_H
#define UMICOM_LANGUAGE_RUNTIME_DOCUMENT_SYNC_H
#include "umicom/language/service.h"
#include "umicom/language_runtime/language_id.h"
#include "umicom/language_runtime/requests/did_open.h"
#include "umicom/language_runtime/requests/did_change.h"
#include "umicom/language_runtime/requests/did_save.h"
#include "umicom/language_runtime/requests/did_close.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the language runtime document state data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageRuntimeDocumentState{char document_id[UMI_LANGUAGE_RUNTIME_ID_CAPACITY],uri[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY],language_id[UMI_LANGUAGE_RUNTIME_ID_CAPACITY];uint64_t version;size_t line_count;int open,dirty;UmiLanguageRuntimeServer*server;}UmiLanguageRuntimeDocumentState;
/**
 * Represent the language runtime document sync data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageRuntimeDocumentSync UmiLanguageRuntimeDocumentSync;
/**
 * Initialise language runtime document sync from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_language_runtime_document_sync_create(UmiLanguageService*l,UmiLanguageRuntimeDocumentSync**out);
/**
 * Release or reset state held by language runtime document sync so the same storage can be
 * reused safely.
 */
void umi_language_runtime_document_sync_destroy(UmiLanguageRuntimeDocumentSync*s);
/**
 * Provide the language runtime document open operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_document_open(UmiLanguageRuntimeDocumentSync*s,UmiLanguageRuntimeServer*server,const char*d,const char*uri,const char*lang,const char*text);
/**
 * Provide the language runtime document change operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_document_change(UmiLanguageRuntimeDocumentSync*s,const char*d,const char*text);
/**
 * Write language runtime document in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_language_runtime_document_save(UmiLanguageRuntimeDocumentSync*s,const char*d,const char*text);
/**
 * Provide the language runtime document close operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_document_close(UmiLanguageRuntimeDocumentSync*s,const char*d);
/**
 * Find language runtime document while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_language_runtime_document_find(const UmiLanguageRuntimeDocumentSync*s,const char*d,UmiLanguageRuntimeDocumentState*out);
/**
 * Provide the language runtime document find by uri operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_document_find_by_uri(const UmiLanguageRuntimeDocumentSync*s,const char*uri,UmiLanguageRuntimeDocumentState*out);
/**
 * Return the number of records represented by language runtime document sync without
 * changing their state.
 */
size_t umi_language_runtime_document_sync_count(const UmiLanguageRuntimeDocumentSync*s);
#ifdef __cplusplus
}
#endif
#endif
