/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/dap_request_internal.h
 *
 * PURPOSE:
 *   Declare the dap request internal contract shared by Framework services and
 *   thin applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework internal DAP helpers. Sammy Hegab, Umicom Foundation, MIT. */
#ifndef UMICOM_PROTOCOL_DAP_REQUEST_INTERNAL_H
#define UMICOM_PROTOCOL_DAP_REQUEST_INTERNAL_H
#include "umicom/protocol/dap_extended.h"
/**
 * Provide the dap request operation used by this module and its client applications.
 */
UmiStatus umi_dap_request(UmiDapClient *client, const char *command,
                          const char *arguments_json,
                          int64_t *out_request_id);
/**
 * Provide the dap thread request operation used by this module and its client
 * applications.
 */
UmiStatus umi_dap_thread_request(UmiDapClient *client, const char *command,
                                 int thread_id, const char *suffix_json,
                                 int64_t *out_request_id);
/**
 * Provide the dap escape operation used by this module and its client applications.
 */
UmiStatus umi_dap_escape(const char *text, char *out, size_t capacity);
#endif
