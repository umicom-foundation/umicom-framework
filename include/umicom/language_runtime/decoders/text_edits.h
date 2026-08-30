/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/decoders/text_edits.h
 *
 * PURPOSE:
 *   Decode TextEdit arrays returned by formatting/code actions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DECODER_TEXT_EDITS_H
#define UMICOM_LANGUAGE_RUNTIME_DECODER_TEXT_EDITS_H
#include "umicom/language_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_language_runtime_decode_text_edits(const char*json,UmiLanguageRuntimeTextEditList*out);
UmiStatus umi_language_runtime_decode_text_edit_array_token(const UmiLanguageRuntimeJsonDocument*d,int token,UmiLanguageRuntimeTextEditList*out);
#ifdef __cplusplus
}
#endif
#endif
