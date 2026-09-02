/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/arguments.h
 *
 * PURPOSE:
 *   Parse language-server profile argument text into bounded argv records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_ARGUMENTS_H
#define UMICOM_LANGUAGE_RUNTIME_ARGUMENTS_H
#include "umicom/language_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_RUNTIME_ARGUMENT_CAPACITY 1024U
/**
 * Represent the language runtime arguments data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageRuntimeArguments { char storage[UMI_LANGUAGE_RUNTIME_MAX_ARGUMENTS][UMI_LANGUAGE_RUNTIME_ARGUMENT_CAPACITY]; const char*values[UMI_LANGUAGE_RUNTIME_MAX_ARGUMENTS]; size_t count; } UmiLanguageRuntimeArguments;
/**
 * Read language runtime arguments into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_language_runtime_arguments_parse(const char*text,UmiLanguageRuntimeArguments*out);
#ifdef __cplusplus
}
#endif
#endif
