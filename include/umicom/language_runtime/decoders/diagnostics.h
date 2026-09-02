/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/decoders/diagnostics.h
 *
 * PURPOSE:
 *   Decode textDocument/publishDiagnostics notifications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DECODER_DIAGNOSTICS_H
#define UMICOM_LANGUAGE_RUNTIME_DECODER_DIAGNOSTICS_H
#include "umicom/language_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the language runtime decode diagnostics operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_decode_diagnostics(const char*json,UmiLanguageRuntimeDiagnosticList*out);
#ifdef __cplusplus
}
#endif
#endif
