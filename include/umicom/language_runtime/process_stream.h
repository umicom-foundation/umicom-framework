/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/process_stream.h
 *
 * PURPOSE:
 *   Own a persistent bidirectional child process for LSP/DAP/interactive tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_PROCESS_STREAM_H
#define UMICOM_LANGUAGE_RUNTIME_PROCESS_STREAM_H
#include "umicom/language_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the language runtime process stream data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageRuntimeProcessStream UmiLanguageRuntimeProcessStream;
/**
 * Represent the language runtime process stream config data shared with callers of this
 * public contract.
 */
typedef struct UmiLanguageRuntimeProcessStreamConfig{const char*program;const char*const*arguments;size_t argument_count;const char*working_directory;int merge_stderr;}UmiLanguageRuntimeProcessStreamConfig;
/**
 * Provide the language runtime process stream start operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_process_stream_start(const UmiLanguageRuntimeProcessStreamConfig*c,UmiLanguageRuntimeProcessStream**out);
/**
 * Release or reset state held by language runtime process stream so the same storage can
 * be reused safely.
 */
void umi_language_runtime_process_stream_destroy(UmiLanguageRuntimeProcessStream*s);
/**
 * Write language runtime process stream in its stable representation and report capacity
 * or input failures to the caller.
 */
UmiStatus umi_language_runtime_process_stream_write(UmiLanguageRuntimeProcessStream*s,const void*b,size_t n);
/**
 * Read language runtime process stream into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus umi_language_runtime_process_stream_read(UmiLanguageRuntimeProcessStream*s,void*out,size_t cap,uint32_t timeout_ms,size_t*n);
/**
 * Provide the language runtime process stream stop operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_process_stream_stop(UmiLanguageRuntimeProcessStream*s,uint32_t timeout_ms);
/**
 * Provide the language runtime process stream is running operation used by this module and
 * its client applications.
 */
int umi_language_runtime_process_stream_is_running(UmiLanguageRuntimeProcessStream*s);
#ifdef __cplusplus
}
#endif
#endif
