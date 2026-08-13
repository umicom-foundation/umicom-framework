/*-----------------------------------------------------------------------------
 * Umicom Framework - internal LSP request construction helpers.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROTOCOL_LSP_REQUEST_INTERNAL_H
#define UMICOM_PROTOCOL_LSP_REQUEST_INTERNAL_H

#include "umicom/protocol/lsp_extended.h"

UmiStatus umi_lsp_request_document(UmiLspClient *client, const char *method,
                                   const char *uri,
                                   int64_t *out_request_id);
UmiStatus umi_lsp_request_position(UmiLspClient *client, const char *method,
                                   const char *uri, UmiLspPosition position,
                                   const char *suffix_json,
                                   int64_t *out_request_id);
UmiStatus umi_lsp_request_range(UmiLspClient *client, const char *method,
                                const char *uri, UmiLspRange range,
                                const char *suffix_json,
                                int64_t *out_request_id);
UmiStatus umi_lsp_escape_text(const char *text, char *out_text,
                              size_t capacity);

#endif
