/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/protocol/client.h
 *
 * PURPOSE:
 *   Manage JSON-RPC request identifiers and framed messages over a Framework protocol transport.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROTOCOL_CLIENT_H
#define UMICOM_PROTOCOL_CLIENT_H

#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/protocol/json_rpc.h"
#include "umicom/protocol/transport.h"
#include "umicom/protocol/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the protocol client data shared with callers of this public contract.
 */
typedef struct UmiProtocolClient UmiProtocolClient;

/**
 * Initialise protocol client from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_protocol_client_create(UmiProtocolTransport *transport,
                                     UmiProtocolClient **out_client);
/**
 * Release or reset state held by protocol client so the same storage can be reused safely.
 */
void umi_protocol_client_destroy(UmiProtocolClient *client);
/**
 * Provide the protocol client start operation used by this module and its client
 * applications.
 */
UmiStatus umi_protocol_client_start(UmiProtocolClient *client);
/**
 * Provide the protocol client stop operation used by this module and its client
 * applications.
 */
UmiStatus umi_protocol_client_stop(UmiProtocolClient *client);
/**
 * Provide the protocol client request operation used by this module and its client
 * applications.
 */
UmiStatus umi_protocol_client_request(UmiProtocolClient *client,
                                      const char *method,
                                      const char *params_json,
                                      int64_t *out_request_id);
/**
 * Provide the protocol client notify operation used by this module and its client
 * applications.
 */
UmiStatus umi_protocol_client_notify(UmiProtocolClient *client,
                                     const char *method,
                                     const char *params_json);
/**
 * Provide the protocol client receive operation used by this module and its client
 * applications.
 */
UmiStatus umi_protocol_client_receive(UmiProtocolClient *client,
                                      UmiJsonRpcMessage *out_message);
/**
 * Provide the protocol client state operation used by this module and its client
 * applications.
 */
UmiProtocolClientState umi_protocol_client_state(
    const UmiProtocolClient *client
);

#ifdef __cplusplus
}
#endif

#endif
