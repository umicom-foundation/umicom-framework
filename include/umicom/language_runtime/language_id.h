/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/language_id.h
 *
 * PURPOSE:
 *   Normalize Framework project-language identities to LSP language IDs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_LANGUAGE_ID_H
#define UMICOM_LANGUAGE_RUNTIME_LANGUAGE_ID_H
#include "umicom/language_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the language runtime normalize language id operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_normalize_language_id(const char*language_id,char*out,size_t cap);
#ifdef __cplusplus
}
#endif
#endif
