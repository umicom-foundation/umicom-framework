/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/pending.h
 *
 * PURPOSE:
 *   Track outstanding JSON-RPC requests by ID, method and document.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_PENDING_H
#define UMICOM_LANGUAGE_RUNTIME_PENDING_H
#include "umicom/language_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiLanguageRuntimePendingRequest{uint64_t request_id;char method[UMI_LANGUAGE_RUNTIME_NAME_CAPACITY];char document_id[UMI_LANGUAGE_RUNTIME_ID_CAPACITY];uint64_t sequence;int active;}UmiLanguageRuntimePendingRequest;
typedef struct UmiLanguageRuntimePendingTable{UmiLanguageRuntimePendingRequest items[UMI_LANGUAGE_RUNTIME_MAX_PENDING_REQUESTS];uint64_t sequence;size_t active_count;}UmiLanguageRuntimePendingTable;
void umi_language_runtime_pending_init(UmiLanguageRuntimePendingTable*t);
UmiStatus umi_language_runtime_pending_add(UmiLanguageRuntimePendingTable*t,uint64_t id,const char*m,const char*d);
UmiStatus umi_language_runtime_pending_take(UmiLanguageRuntimePendingTable*t,uint64_t id,UmiLanguageRuntimePendingRequest*out);
UmiStatus umi_language_runtime_pending_find(const UmiLanguageRuntimePendingTable*t,uint64_t id,UmiLanguageRuntimePendingRequest*out);
#ifdef __cplusplus
}
#endif
#endif
