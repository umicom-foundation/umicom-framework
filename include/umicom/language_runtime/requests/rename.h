/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/requests/rename.h
 *
 * PURPOSE:
 *   Request a provider-generated workspace rename edit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_REQUEST_RENAME_H
#define UMICOM_LANGUAGE_RUNTIME_REQUEST_RENAME_H
#include "umicom/language_runtime/request_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_language_runtime_request_rename(UmiLanguageRuntimeServer*s,const char*d,const char*u,uint32_t l,uint32_t c,const char*new_name,uint64_t*out);
#ifdef __cplusplus
}
#endif
#endif
