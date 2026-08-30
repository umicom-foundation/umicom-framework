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
typedef struct UmiLanguageRuntimeJsonWriter { char *buffer; size_t capacity,length; UmiStatus status; } UmiLanguageRuntimeJsonWriter;
void umi_language_runtime_json_writer_init(UmiLanguageRuntimeJsonWriter*w,char*b,size_t c);
UmiStatus umi_language_runtime_json_writer_raw(UmiLanguageRuntimeJsonWriter*w,const char*t);
UmiStatus umi_language_runtime_json_writer_string(UmiLanguageRuntimeJsonWriter*w,const char*t);
UmiStatus umi_language_runtime_json_writer_uint64(UmiLanguageRuntimeJsonWriter*w,uint64_t v);
UmiStatus umi_language_runtime_json_writer_int64(UmiLanguageRuntimeJsonWriter*w,int64_t v);
UmiStatus umi_language_runtime_json_writer_bool(UmiLanguageRuntimeJsonWriter*w,int v);
#ifdef __cplusplus
}
#endif
#endif
