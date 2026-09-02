/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/transport.h
 *
 * PURPOSE:
 *   Define reusable byte transport abstraction for stdio, memory and future remote LSP.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_TRANSPORT_H
#define UMICOM_LANGUAGE_RUNTIME_TRANSPORT_H
#include "umicom/language_runtime/process_stream.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the language runtime transport data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageRuntimeTransport{void*instance;UmiStatus(*write)(void*,const void*,size_t);UmiStatus(*read)(void*,void*,size_t,uint32_t,size_t*);UmiStatus(*stop)(void*,uint32_t);int(*is_running)(void*);void(*destroy)(void*);}UmiLanguageRuntimeTransport;
/**
 * Check that language runtime transport satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_language_runtime_transport_validate(const UmiLanguageRuntimeTransport*t);
/**
 * Provide the language runtime transport from process operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_transport_from_process(UmiLanguageRuntimeProcessStream*s,UmiLanguageRuntimeTransport*out);
#ifdef __cplusplus
}
#endif
#endif
