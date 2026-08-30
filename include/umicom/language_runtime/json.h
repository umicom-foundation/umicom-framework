/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/json.h
 *
 * PURPOSE:
 *   Provide a bounded dependency-free JSON token reader for JSON-RPC/LSP messages.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_JSON_H
#define UMICOM_LANGUAGE_RUNTIME_JSON_H
#include "umicom/language_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiLanguageRuntimeJsonTokenType { UMI_LANGUAGE_RUNTIME_JSON_UNDEFINED=0, UMI_LANGUAGE_RUNTIME_JSON_OBJECT=1, UMI_LANGUAGE_RUNTIME_JSON_ARRAY=2, UMI_LANGUAGE_RUNTIME_JSON_STRING=3, UMI_LANGUAGE_RUNTIME_JSON_PRIMITIVE=4 } UmiLanguageRuntimeJsonTokenType;
typedef struct UmiLanguageRuntimeJsonToken { UmiLanguageRuntimeJsonTokenType type; int start; int end; int size; int parent; } UmiLanguageRuntimeJsonToken;
typedef struct UmiLanguageRuntimeJsonDocument { const char *json; UmiLanguageRuntimeJsonToken tokens[UMI_LANGUAGE_RUNTIME_MAX_TOKENS]; size_t token_count; } UmiLanguageRuntimeJsonDocument;
UmiStatus umi_language_runtime_json_parse(const char *json,UmiLanguageRuntimeJsonDocument *out_document);
int umi_language_runtime_json_object_get(const UmiLanguageRuntimeJsonDocument *document,int object_token,const char *key);
size_t umi_language_runtime_json_object_count(const UmiLanguageRuntimeJsonDocument *document,int object_token);
UmiStatus umi_language_runtime_json_object_entry_at(const UmiLanguageRuntimeJsonDocument *document,int object_token,size_t index,int *out_key_token,int *out_value_token);
int umi_language_runtime_json_array_at(const UmiLanguageRuntimeJsonDocument *document,int array_token,size_t index);
size_t umi_language_runtime_json_array_count(const UmiLanguageRuntimeJsonDocument *document,int array_token);
UmiStatus umi_language_runtime_json_string(const UmiLanguageRuntimeJsonDocument *document,int token,char *out_text,size_t capacity);
UmiStatus umi_language_runtime_json_int64(const UmiLanguageRuntimeJsonDocument *document,int token,int64_t *out_value);
UmiStatus umi_language_runtime_json_bool(const UmiLanguageRuntimeJsonDocument *document,int token,int *out_value);
int umi_language_runtime_json_is_null(const UmiLanguageRuntimeJsonDocument *document,int token);
#ifdef __cplusplus
}
#endif
#endif
