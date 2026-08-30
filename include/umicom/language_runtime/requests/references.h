/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/requests/references.h
 *
 * PURPOSE:
 *   Send textDocument/references with declaration policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_REQUEST_REFERENCES_H
#define UMICOM_LANGUAGE_RUNTIME_REQUEST_REFERENCES_H
#include "umicom/language_runtime/request_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_language_runtime_request_references(UmiLanguageRuntimeServer*s,const char*d,const char*u,uint32_t l,uint32_t c,int include_decl,uint64_t*out);
#ifdef __cplusplus
}
#endif
#endif
