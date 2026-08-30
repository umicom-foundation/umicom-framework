/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/requests/hover.h
 *
 * PURPOSE:
 *   Send the LSP textDocument/hover request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_REQUEST_HOVER_H
#define UMICOM_LANGUAGE_RUNTIME_REQUEST_HOVER_H
#include "umicom/language_runtime/request_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_language_runtime_request_hover(UmiLanguageRuntimeServer*s,const char*document_id,const char*uri,uint32_t line,uint32_t column,uint64_t*out_id);
#ifdef __cplusplus
}
#endif
#endif
