/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/lsp_request_internal.h
 *
 * PURPOSE:
 *   Declare the lsp request internal contract shared by Framework services and
 *   thin applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework - internal LSP request construction helpers.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROTOCOL_LSP_REQUEST_INTERNAL_H
#define UMICOM_PROTOCOL_LSP_REQUEST_INTERNAL_H

#include "umicom/protocol/lsp_extended.h"

/**
 * Provide the lsp request document operation used by this module and its client
 * applications.
 */
UmiStatus umi_lsp_request_document(UmiLspClient *client, const char *method,
                                   const char *uri,
                                   int64_t *out_request_id);
/**
 * Provide the lsp request position operation used by this module and its client
 * applications.
 */
UmiStatus umi_lsp_request_position(UmiLspClient *client, const char *method,
                                   const char *uri, UmiLspPosition position,
                                   const char *suffix_json,
                                   int64_t *out_request_id);
/**
 * Provide the lsp request range operation used by this module and its client applications.
 */
UmiStatus umi_lsp_request_range(UmiLspClient *client, const char *method,
                                const char *uri, UmiLspRange range,
                                const char *suffix_json,
                                int64_t *out_request_id);
/**
 * Provide the lsp escape text operation used by this module and its client applications.
 */
UmiStatus umi_lsp_escape_text(const char *text, char *out_text,
                              size_t capacity);

#endif
