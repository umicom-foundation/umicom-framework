/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/decoders/hover.h
 *
 * PURPOSE:
 *   Decode Hover contents and optional range.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DECODER_HOVER_H
#define UMICOM_LANGUAGE_RUNTIME_DECODER_HOVER_H
#include "umicom/language_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the language runtime decode hover operation used by this module and its client
 * applications.
 */
/* Compatibility output joins supported blocks as literal text. No truncation:
 * content exceeding the fixed output capacity is rejected. Invalid envelopes,
 * missing contents and malformed optional ranges fail with a cleared output.
 * This API cannot correlate request IDs; use UmiLanguageHoverDocumentReadResponse
 * for a standalone response, or correlate in the existing dispatcher first. */
UmiStatus umi_language_runtime_decode_hover(const char*json,UmiLanguageRuntimeHoverResult*out);
#ifdef __cplusplus
}
#endif
#endif
