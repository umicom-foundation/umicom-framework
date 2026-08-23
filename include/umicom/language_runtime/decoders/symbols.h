/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/decoders/symbols.h
 *
 * PURPOSE:
 *   Decode DocumentSymbol/SymbolInformation arrays.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DECODER_SYMBOLS_H
#define UMICOM_LANGUAGE_RUNTIME_DECODER_SYMBOLS_H
#include "umicom/language_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_language_runtime_decode_symbols(const char*json,const char*default_uri,UmiLanguageRuntimeSymbolList*out);
#ifdef __cplusplus
}
#endif
#endif
