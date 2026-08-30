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
typedef struct UmiLanguageRuntimeProcessStream UmiLanguageRuntimeProcessStream;
typedef struct UmiLanguageRuntimeProcessStreamConfig{const char*program;const char*const*arguments;size_t argument_count;const char*working_directory;int merge_stderr;}UmiLanguageRuntimeProcessStreamConfig;
UmiStatus umi_language_runtime_process_stream_start(const UmiLanguageRuntimeProcessStreamConfig*c,UmiLanguageRuntimeProcessStream**out);
void umi_language_runtime_process_stream_destroy(UmiLanguageRuntimeProcessStream*s);
UmiStatus umi_language_runtime_process_stream_write(UmiLanguageRuntimeProcessStream*s,const void*b,size_t n);
UmiStatus umi_language_runtime_process_stream_read(UmiLanguageRuntimeProcessStream*s,void*out,size_t cap,uint32_t timeout_ms,size_t*n);
UmiStatus umi_language_runtime_process_stream_stop(UmiLanguageRuntimeProcessStream*s,uint32_t timeout_ms);
int umi_language_runtime_process_stream_is_running(UmiLanguageRuntimeProcessStream*s);
#ifdef __cplusplus
}
#endif
#endif
