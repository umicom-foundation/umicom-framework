/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/json_writer.h
 *
 * PURPOSE:
 *   Build bounded JSON payloads with correct string escaping.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_JSON_WRITER_H
#define UMICOM_LANGUAGE_RUNTIME_JSON_WRITER_H
#include "umicom/language_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the language runtime json writer data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageRuntimeJsonWriter { char *buffer; size_t capacity,length; UmiStatus status; } UmiLanguageRuntimeJsonWriter;
/**
 * Initialise language runtime json writer from caller-provided values so later operations
 * receive a known state.
 */
void umi_language_runtime_json_writer_init(UmiLanguageRuntimeJsonWriter*w,char*b,size_t c);
/**
 * Provide the language runtime json writer raw operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_json_writer_raw(UmiLanguageRuntimeJsonWriter*w,const char*t);
/**
 * Provide the language runtime json writer string operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_json_writer_string(UmiLanguageRuntimeJsonWriter*w,const char*t);
/**
 * Provide the language runtime json writer uint64 operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_json_writer_uint64(UmiLanguageRuntimeJsonWriter*w,uint64_t v);
/**
 * Provide the language runtime json writer int64 operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_json_writer_int64(UmiLanguageRuntimeJsonWriter*w,int64_t v);
/**
 * Provide the language runtime json writer bool operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_json_writer_bool(UmiLanguageRuntimeJsonWriter*w,int v);
#ifdef __cplusplus
}
#endif
#endif
