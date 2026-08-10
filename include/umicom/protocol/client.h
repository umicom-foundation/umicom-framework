/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/protocol/client.h
 *
 * PURPOSE:
 *   Manage JSON-RPC request identifiers and framed messages over a Framework protocol transport.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiProtocolClient UmiProtocolClient;

UmiStatus umi_protocol_client_create(UmiProtocolTransport *transport,
                                     UmiProtocolClient **out_client);
void umi_protocol_client_destroy(UmiProtocolClient *client);
UmiStatus umi_protocol_client_start(UmiProtocolClient *client);
UmiStatus umi_protocol_client_stop(UmiProtocolClient *client);
UmiStatus umi_protocol_client_request(UmiProtocolClient *client,
                                      const char *method,
                                      const char *params_json,
                                      int64_t *out_request_id);
UmiStatus umi_protocol_client_notify(UmiProtocolClient *client,
                                     const char *method,
                                     const char *params_json);
UmiStatus umi_protocol_client_receive(UmiProtocolClient *client,
                                      UmiJsonRpcMessage *out_message);
UmiProtocolClientState umi_protocol_client_state(
    const UmiProtocolClient *client
);

#ifdef __cplusplus
}
#endif

#endif
