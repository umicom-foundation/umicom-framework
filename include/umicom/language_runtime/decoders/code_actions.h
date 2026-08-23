/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/decoders/code_actions.h
 *
 * PURPOSE:
 *   Decode CodeAction/Command result arrays.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DECODER_CODE_ACTIONS_H
#define UMICOM_LANGUAGE_RUNTIME_DECODER_CODE_ACTIONS_H
#include "umicom/language_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_language_runtime_decode_code_actions(const char*json,UmiLanguageRuntimeCodeActionList*out);
#ifdef __cplusplus
}
#endif
#endif
