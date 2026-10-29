/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/decoders/initialize.h
 *
 * PURPOSE:
 *   Decode initialize-result server capability flags.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DECODER_INITIALIZE_H
#define UMICOM_LANGUAGE_RUNTIME_DECODER_INITIALIZE_H
#include "umicom/language_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Initialise language runtime decode from caller-provided values so later operations
 * receive a known state.
 */
/* Read a bounded result.capabilities object and publish availability flags only
 * after every recognised field is accepted. Refusal leaves out unchanged.
 * Missing/null/false fields remain disabled for compatibility. Enabled values
 * require an options object, an allowed Boolean, or text-sync integer 1 or 2;
 * integer 0 disables sync. Arrays, strings and other numbers are refused.
 * Duplicate result, capabilities or recognised field names are refused, including
 * escaped spellings. Unknown fields are ignored after JSON grammar validation.
 * This is an availability projection: it does not validate every nested option,
 * select an encoding, correlate IDs or validate the JSON-RPC envelope. The
 * lifecycle/response service performs those envelope and request checks.
 * Input is NUL-terminated and shorter than UMI_LANGUAGE_RUNTIME_JSON_CAPACITY;
 * parsing is bounded to 16,384 nodes and 64 container levels. */
UmiStatus umi_language_runtime_decode_initialize(const char*json,UmiLanguageRuntimeInitializeResult*out);
#ifdef __cplusplus
}
#endif
#endif
