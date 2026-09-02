/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/requests/document_symbols.h
 *
 * PURPOSE:
 *   Send the LSP textDocument/documentSymbol request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_REQUEST_DOCUMENT_SYMBOLS_H
#define UMICOM_LANGUAGE_RUNTIME_REQUEST_DOCUMENT_SYMBOLS_H
#include "umicom/language_runtime/request_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the language runtime request document symbols operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_request_document_symbols(UmiLanguageRuntimeServer*s,const char*document_id,const char*uri,uint64_t*out_id);
#ifdef __cplusplus
}
#endif
#endif
