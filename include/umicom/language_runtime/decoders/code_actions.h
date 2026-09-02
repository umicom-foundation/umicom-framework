/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/decoders/code_actions.h
 *
 * PURPOSE:
 *   Decode CodeAction/Command result arrays.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DECODER_CODE_ACTIONS_H
#define UMICOM_LANGUAGE_RUNTIME_DECODER_CODE_ACTIONS_H
#include "umicom/language_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the language runtime decode code actions operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_decode_code_actions(const char*json,UmiLanguageRuntimeCodeActionList*out);
#ifdef __cplusplus
}
#endif
#endif
