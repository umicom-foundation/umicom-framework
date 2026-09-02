/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/decoders/workspace_edit.h
 *
 * PURPOSE:
 *   Decode WorkspaceEdit changes into URI + TextEdit records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DECODER_WORKSPACE_EDIT_H
#define UMICOM_LANGUAGE_RUNTIME_DECODER_WORKSPACE_EDIT_H
#include "umicom/language_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the language runtime decode workspace edit operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_decode_workspace_edit(const char*json,UmiLanguageRuntimeWorkspaceEdit*out);
#ifdef __cplusplus
}
#endif
#endif
