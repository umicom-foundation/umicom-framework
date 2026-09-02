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
/**
 * List the named language runtime json token type values accepted by this public contract.
 */
typedef enum UmiLanguageRuntimeJsonTokenType { UMI_LANGUAGE_RUNTIME_JSON_UNDEFINED=0, UMI_LANGUAGE_RUNTIME_JSON_OBJECT=1, UMI_LANGUAGE_RUNTIME_JSON_ARRAY=2, UMI_LANGUAGE_RUNTIME_JSON_STRING=3, UMI_LANGUAGE_RUNTIME_JSON_PRIMITIVE=4 } UmiLanguageRuntimeJsonTokenType;
/**
 * Represent the language runtime json token data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageRuntimeJsonToken { UmiLanguageRuntimeJsonTokenType type; int start; int end; int size; int parent; } UmiLanguageRuntimeJsonToken;
/**
 * Represent the language runtime json document data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageRuntimeJsonDocument { const char *json; UmiLanguageRuntimeJsonToken tokens[UMI_LANGUAGE_RUNTIME_MAX_TOKENS]; size_t token_count; } UmiLanguageRuntimeJsonDocument;
/**
 * Read language runtime json into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_language_runtime_json_parse(const char *json,UmiLanguageRuntimeJsonDocument *out_document);
/**
 * Provide the language runtime json object get operation used by this module and its
 * client applications.
 */
int umi_language_runtime_json_object_get(const UmiLanguageRuntimeJsonDocument *document,int object_token,const char *key);
/**
 * Return the number of records represented by language runtime json object without
 * changing their state.
 */
size_t umi_language_runtime_json_object_count(const UmiLanguageRuntimeJsonDocument *document,int object_token);
/**
 * Find language runtime json object entry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_language_runtime_json_object_entry_at(const UmiLanguageRuntimeJsonDocument *document,int object_token,size_t index,int *out_key_token,int *out_value_token);
/**
 * Find language runtime json array while leaving the underlying catalogue or model owned
 * by this module.
 */
int umi_language_runtime_json_array_at(const UmiLanguageRuntimeJsonDocument *document,int array_token,size_t index);
/**
 * Return the number of records represented by language runtime json array without changing
 * their state.
 */
size_t umi_language_runtime_json_array_count(const UmiLanguageRuntimeJsonDocument *document,int array_token);
/**
 * Provide the language runtime json string operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_json_string(const UmiLanguageRuntimeJsonDocument *document,int token,char *out_text,size_t capacity);
/**
 * Provide the language runtime json int64 operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_json_int64(const UmiLanguageRuntimeJsonDocument *document,int token,int64_t *out_value);
/**
 * Provide the language runtime json bool operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_json_bool(const UmiLanguageRuntimeJsonDocument *document,int token,int *out_value);
/**
 * Provide the language runtime json is null operation used by this module and its client
 * applications.
 */
int umi_language_runtime_json_is_null(const UmiLanguageRuntimeJsonDocument *document,int token);
#ifdef __cplusplus
}
#endif
#endif
