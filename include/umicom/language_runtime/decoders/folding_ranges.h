/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/decoders/folding_ranges.h
 *
 * PURPOSE:
 *   Decode textDocument/foldingRange arrays.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DECODER_FOLDING_RANGES_H
#define UMICOM_LANGUAGE_RUNTIME_DECODER_FOLDING_RANGES_H
#include "umicom/language_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the language runtime decode folding ranges operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_decode_folding_ranges(const char*json,UmiLanguageRuntimeFoldingRangeList*out);
#ifdef __cplusplus
}
#endif
#endif
