/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/requests/folding_ranges.h
 *
 * PURPOSE:
 *   Send the LSP textDocument/foldingRange request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_REQUEST_FOLDING_RANGES_H
#define UMICOM_LANGUAGE_RUNTIME_REQUEST_FOLDING_RANGES_H
#include "umicom/language_runtime/request_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_language_runtime_request_folding_ranges(UmiLanguageRuntimeServer*s,const char*document_id,const char*uri,uint64_t*out_id);
#ifdef __cplusplus
}
#endif
#endif
