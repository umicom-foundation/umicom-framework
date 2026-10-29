/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/framing.h
 *
 * PURPOSE:
 *   Encode/decode incremental LSP Content-Length framing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_FRAMING_H
#define UMICOM_LANGUAGE_RUNTIME_FRAMING_H
#include "umicom/language_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the language runtime framer data shared with callers of this public contract.
 */
typedef struct UmiLanguageRuntimeFramer { char buffer[UMI_LANGUAGE_RUNTIME_FRAME_CAPACITY]; size_t length,expected_body,header_length; int header_ready; } UmiLanguageRuntimeFramer;
/**
 * Initialise language runtime framer from caller-provided values so later operations
 * receive a known state.
 */
void umi_language_runtime_framer_init(UmiLanguageRuntimeFramer*f);
/**
 * Write language runtime frame in its stable representation and report capacity or input
 * failures to the caller.
 */
/* Input is a NUL-terminated JSON byte string shorter than JSON_CAPACITY.
 * Capacity includes the final NUL. Refusal preserves destination and length;
 * encoding may reuse the input storage if destination capacity is sufficient. */
UmiStatus umi_language_runtime_frame_encode(const char*json,char*out,size_t cap,size_t*out_len);
/**
 * Provide the language runtime framer feed operation used by this module and its client
 * applications.
 */
/* Feed accepts arbitrary byte fragments, including several complete frames.
 * Length/capacity and literal-NUL refusal preserve the previous buffered state.
 * A malformed complete header remains an error until init resets the framer.
 * Header fields are ASCII; Content-Length is unique, unsigned decimal bytes.
 * Content-Type, when declaring a charset, must select UTF-8 (or utf8). */
UmiStatus umi_language_runtime_framer_feed(UmiLanguageRuntimeFramer*f,const void*b,size_t n);
/**
 * Provide the language runtime framer has message operation used by this module and its
 * client applications.
 */
/* Inspect without consuming: OK means a complete frame, NOT_FOUND means more
 * bytes are needed, PARSE_ERROR means a buffered malformed header. Consumers
 * must inspect status before waiting for more input after a successful pop.
 * Treat the fields as private state despite their retained public layout. */
UmiStatus umi_language_runtime_framer_status(const UmiLanguageRuntimeFramer *framer);
int umi_language_runtime_framer_has_message(const UmiLanguageRuntimeFramer*f);
/**
 * Provide the language runtime framer pop operation used by this module and its client
 * applications.
 */
/* Output and out_len must not overlap framer storage or each other. Refusal
 * leaves them unchanged. A valid frame is delivered even when the following
 * header is malformed; that next error is available through framer_status.
 * A zero-byte body is a complete frame; JSON validity belongs to message_parse. */
UmiStatus umi_language_runtime_framer_pop(UmiLanguageRuntimeFramer*f,char*out,size_t cap,size_t*out_len);
#ifdef __cplusplus
}
#endif
#endif
