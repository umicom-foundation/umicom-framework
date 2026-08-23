/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/decoders/folding_ranges.h
 *
 * PURPOSE:
 *   Decode textDocument/foldingRange arrays.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DECODER_FOLDING_RANGES_H
#define UMICOM_LANGUAGE_RUNTIME_DECODER_FOLDING_RANGES_H
#include "umicom/language_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_language_runtime_decode_folding_ranges(const char*json,UmiLanguageRuntimeFoldingRangeList*out);
#ifdef __cplusplus
}
#endif
#endif
