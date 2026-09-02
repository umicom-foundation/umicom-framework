/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/decoders/symbols.h
 *
 * PURPOSE:
 *   Decode DocumentSymbol/SymbolInformation arrays.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DECODER_SYMBOLS_H
#define UMICOM_LANGUAGE_RUNTIME_DECODER_SYMBOLS_H
#include "umicom/language_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the language runtime decode symbols operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_decode_symbols(const char*json,const char*default_uri,UmiLanguageRuntimeSymbolList*out);
#ifdef __cplusplus
}
#endif
#endif
