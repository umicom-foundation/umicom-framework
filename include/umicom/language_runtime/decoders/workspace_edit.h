/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/decoders/workspace_edit.h
 *
 * PURPOSE:
 *   Decode WorkspaceEdit changes into URI + TextEdit records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DECODER_WORKSPACE_EDIT_H
#define UMICOM_LANGUAGE_RUNTIME_DECODER_WORKSPACE_EDIT_H
#include "umicom/language_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_language_runtime_decode_workspace_edit(const char*json,UmiLanguageRuntimeWorkspaceEdit*out);
#ifdef __cplusplus
}
#endif
#endif
