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
/**
 * Represent the language runtime memory transport data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageRuntimeMemoryTransport UmiLanguageRuntimeMemoryTransport;
/**
 * Initialise language runtime memory transport from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_language_runtime_memory_transport_create(UmiLanguageRuntimeMemoryTransport**m,UmiLanguageRuntimeTransport*out);
/**
 * Read language runtime memory transport push into validated module state and return a
 * status when input cannot be used.
 */
UmiStatus umi_language_runtime_memory_transport_push_read(UmiLanguageRuntimeMemoryTransport*m,const void*b,size_t n);
/**
 * Provide the language runtime memory transport written operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_memory_transport_written(const UmiLanguageRuntimeMemoryTransport*m,char*out,size_t cap,size_t*n);
#ifdef __cplusplus
}
#endif
#endif
