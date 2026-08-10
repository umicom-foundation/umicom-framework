/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/protocol/lsp.h
 *
 * PURPOSE:
 *   Create Language Server Protocol initialise, document, completion and shutdown messages.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROTOCOL_LSP_H
#define UMICOM_PROTOCOL_LSP_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/protocol/client.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiLspPosition {
    uint32_t line;
    uint32_t character;
} UmiLspPosition;

typedef struct UmiLspClient {
    UmiProtocolClient *client;
    int initialized;
    char root_uri[UMI_PROTOCOL_URI_CAPACITY];
} UmiLspClient;

UmiStatus umi_lsp_client_init(UmiLspClient *client,
                              UmiProtocolClient *protocol_client,
                              const char *root_uri);
UmiStatus umi_lsp_initialize(UmiLspClient *client,
                             int64_t process_id,
                             int64_t *out_request_id);
UmiStatus umi_lsp_initialized(UmiLspClient *client);
UmiStatus umi_lsp_did_open(UmiLspClient *client,
                           const char *uri,
                           const char *language_id,
                           int version,
                           const char *text);
UmiStatus umi_lsp_did_change(UmiLspClient *client,
                             const char *uri,
                             int version,
                             const char *text);
UmiStatus umi_lsp_completion(UmiLspClient *client,
                             const char *uri,
                             UmiLspPosition position,
                             int64_t *out_request_id);
UmiStatus umi_lsp_shutdown(UmiLspClient *client,
                           int64_t *out_request_id);

#ifdef __cplusplus
}
#endif

#endif
