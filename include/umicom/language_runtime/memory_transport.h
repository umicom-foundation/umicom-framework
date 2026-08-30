/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/memory_transport.h
 *
 * PURPOSE:
 *   Provide deterministic in-memory transport for protocol tests and replay.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_MEMORY_TRANSPORT_H
#define UMICOM_LANGUAGE_RUNTIME_MEMORY_TRANSPORT_H
#include "umicom/language_runtime/transport.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiLanguageRuntimeMemoryTransport UmiLanguageRuntimeMemoryTransport;
UmiStatus umi_language_runtime_memory_transport_create(UmiLanguageRuntimeMemoryTransport**m,UmiLanguageRuntimeTransport*out);
UmiStatus umi_language_runtime_memory_transport_push_read(UmiLanguageRuntimeMemoryTransport*m,const void*b,size_t n);
UmiStatus umi_language_runtime_memory_transport_written(const UmiLanguageRuntimeMemoryTransport*m,char*out,size_t cap,size_t*n);
#ifdef __cplusplus
}
#endif
#endif
