/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/client.c
 *
 * PURPOSE:
 *   Implement a deterministic in-process JSON-RPC client over the transport contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/protocol/client.h"

#include <stdlib.h>

#include "umicom/platform/threading.h"

struct UmiProtocolClient {
    UmiProtocolTransport *transport;
    UmiProtocolClientState state;
    int64_t next_request_id;
    UmiMutex *mutex;
};

UmiStatus umi_protocol_client_create(UmiProtocolTransport *transport,
                                     UmiProtocolClient **out_client)
{
    UmiProtocolClient *client;
    if (transport == NULL || out_client == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_client = NULL;
    client = (UmiProtocolClient *)calloc(1U, sizeof(*client));
    if (client == NULL ||
        umi_mutex_create(&client->mutex) != UMI_STATUS_OK) {
        free(client);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    client->transport = transport;
    client->state = UMI_PROTOCOL_CLIENT_CREATED;
    client->next_request_id = 1;
    *out_client = client;
    return UMI_STATUS_OK;
}

void umi_protocol_client_destroy(UmiProtocolClient *client)
{
    if (client != NULL) {
        umi_mutex_destroy(client->mutex);
        free(client);
    }
}

UmiStatus umi_protocol_client_start(UmiProtocolClient *client)
{
    if (client == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    client->state = UMI_PROTOCOL_CLIENT_READY;
    return UMI_STATUS_OK;
}

UmiStatus umi_protocol_client_stop(UmiProtocolClient *client)
{
    if (client == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    client->state = UMI_PROTOCOL_CLIENT_STOPPED;
    return UMI_STATUS_OK;
}

UmiStatus umi_protocol_client_request(UmiProtocolClient *client,
                                      const char *method,
                                      const char *params_json,
                                      int64_t *out_request_id)
{
    char json[UMI_PROTOCOL_MESSAGE_CAPACITY];
    int64_t id;
    UmiStatus status;
    if (client == NULL || client->state != UMI_PROTOCOL_CLIENT_READY) {
        return UMI_STATUS_INVALID_STATE;
    }
    (void)umi_mutex_lock(client->mutex);
    id = client->next_request_id++;
    (void)umi_mutex_unlock(client->mutex);
    status = umi_json_rpc_build_request(id,
                                        method,
                                        params_json,
                                        json,
                                        sizeof(json));
    if (status == UMI_STATUS_OK) {
        status = umi_protocol_transport_send(client->transport, json);
    }
    if (status == UMI_STATUS_OK && out_request_id != NULL) {
        *out_request_id = id;
    }
    return status;
}

UmiStatus umi_protocol_client_notify(UmiProtocolClient *client,
                                     const char *method,
                                     const char *params_json)
{
    char json[UMI_PROTOCOL_MESSAGE_CAPACITY];
    UmiStatus status;
    if (client == NULL || client->state != UMI_PROTOCOL_CLIENT_READY) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_json_rpc_build_notification(method,
                                             params_json,
                                             json,
                                             sizeof(json));
    return status == UMI_STATUS_OK
        ? umi_protocol_transport_send(client->transport, json)
        : status;
}

UmiStatus umi_protocol_client_receive(UmiProtocolClient *client,
                                      UmiJsonRpcMessage *out_message)
{
    char json[UMI_PROTOCOL_MESSAGE_CAPACITY];
    UmiStatus status;
    if (client == NULL || out_message == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_protocol_transport_receive(client->transport,
                                            json,
                                            sizeof(json));
    return status == UMI_STATUS_OK
        ? umi_json_rpc_parse(json, out_message)
        : status;
}

UmiProtocolClientState umi_protocol_client_state(
    const UmiProtocolClient *client)
{
    return client != NULL
        ? client->state
        : UMI_PROTOCOL_CLIENT_FAILED;
}
