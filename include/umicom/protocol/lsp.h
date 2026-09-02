/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/protocol/lsp.h
 *
 * PURPOSE:
 *   Create Language Server Protocol initialise, document, completion and shutdown messages.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the lsp position data shared with callers of this public contract.
 */
typedef struct UmiLspPosition {
    uint32_t line;
    uint32_t character;
} UmiLspPosition;

/**
 * Represent the lsp client data shared with callers of this public contract.
 */
typedef struct UmiLspClient {
    UmiProtocolClient *client;
    int initialized;
    char root_uri[UMI_PROTOCOL_URI_CAPACITY];
} UmiLspClient;

/**
 * Initialise lsp client from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_lsp_client_init(UmiLspClient *client,
                              UmiProtocolClient *protocol_client,
                              const char *root_uri);
/**
 * Initialise lsp from caller-provided values so later operations receive a known state.
 */
UmiStatus umi_lsp_initialize(UmiLspClient *client,
                             int64_t process_id,
                             int64_t *out_request_id);
/**
 * Provide the lsp initialized operation used by this module and its client applications.
 */
UmiStatus umi_lsp_initialized(UmiLspClient *client);
/**
 * Provide the lsp did open operation used by this module and its client applications.
 */
UmiStatus umi_lsp_did_open(UmiLspClient *client,
                           const char *uri,
                           const char *language_id,
                           int version,
                           const char *text);
/**
 * Provide the lsp did change operation used by this module and its client applications.
 */
UmiStatus umi_lsp_did_change(UmiLspClient *client,
                             const char *uri,
                             int version,
                             const char *text);
/**
 * Provide the lsp completion operation used by this module and its client applications.
 */
UmiStatus umi_lsp_completion(UmiLspClient *client,
                             const char *uri,
                             UmiLspPosition position,
                             int64_t *out_request_id);
/**
 * Provide the lsp shutdown operation used by this module and its client applications.
 */
UmiStatus umi_lsp_shutdown(UmiLspClient *client,
                           int64_t *out_request_id);

#ifdef __cplusplus
}
#endif

#endif
