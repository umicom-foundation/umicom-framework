/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/language_id.h
 *
 * PURPOSE:
 *   Normalize Framework project-language identities to LSP language IDs.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_LANGUAGE_ID_H
#define UMICOM_LANGUAGE_RUNTIME_LANGUAGE_ID_H
#include "umicom/language_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_language_runtime_normalize_language_id(const char*language_id,char*out,size_t cap);
#ifdef __cplusplus
}
#endif
#endif
