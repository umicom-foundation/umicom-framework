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
typedef struct UmiLanguageRuntimeDocumentState{char document_id[UMI_LANGUAGE_RUNTIME_ID_CAPACITY],uri[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY],language_id[UMI_LANGUAGE_RUNTIME_ID_CAPACITY];uint64_t version;size_t line_count;int open,dirty;UmiLanguageRuntimeServer*server;}UmiLanguageRuntimeDocumentState;
typedef struct UmiLanguageRuntimeDocumentSync UmiLanguageRuntimeDocumentSync;
UmiStatus umi_language_runtime_document_sync_create(UmiLanguageService*l,UmiLanguageRuntimeDocumentSync**out);
void umi_language_runtime_document_sync_destroy(UmiLanguageRuntimeDocumentSync*s);
UmiStatus umi_language_runtime_document_open(UmiLanguageRuntimeDocumentSync*s,UmiLanguageRuntimeServer*server,const char*d,const char*uri,const char*lang,const char*text);
UmiStatus umi_language_runtime_document_change(UmiLanguageRuntimeDocumentSync*s,const char*d,const char*text);
UmiStatus umi_language_runtime_document_save(UmiLanguageRuntimeDocumentSync*s,const char*d,const char*text);
UmiStatus umi_language_runtime_document_close(UmiLanguageRuntimeDocumentSync*s,const char*d);
UmiStatus umi_language_runtime_document_find(const UmiLanguageRuntimeDocumentSync*s,const char*d,UmiLanguageRuntimeDocumentState*out);
UmiStatus umi_language_runtime_document_find_by_uri(const UmiLanguageRuntimeDocumentSync*s,const char*uri,UmiLanguageRuntimeDocumentState*out);
size_t umi_language_runtime_document_sync_count(const UmiLanguageRuntimeDocumentSync*s);
#ifdef __cplusplus
}
#endif
#endif
