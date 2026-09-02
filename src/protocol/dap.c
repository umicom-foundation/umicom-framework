/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/dap.c
 *
 * PURPOSE:
 *   Implement core Debug Adapter Protocol request generation over JSON-RPC transport.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/protocol/dap.h"

#include <stdio.h>
#include <string.h>

#include "umicom/protocol/json.h"

/*
 * Initialise dap client from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_dap_client_init(UmiDapClient *client,
                              UmiProtocolClient *protocol_client)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (client == NULL || protocol_client == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(client, 0, sizeof(*client));
    client->client = protocol_client;
    return UMI_STATUS_OK;
}

/* Initialise dap from caller-provided values so later operations receive a known state. */
UmiStatus umi_dap_initialize(UmiDapClient *client,
                             const char *adapter_id,
                             int64_t *out_request_id)
{
    char escaped[512];
    char params[2048];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (client == NULL || adapter_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_json_escape(adapter_id, escaped, sizeof(escaped));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    (void)snprintf(params,
                   sizeof(params),
                   "{\"clientID\":\"umicom-studio\",\"adapterID\":\"%s\","
                   "\"linesStartAt1\":true,\"columnsStartAt1\":true,"
                   "\"supportsVariableType\":true}",
                   escaped);
    status = umi_protocol_client_request(client->client,
                                         "initialize",
                                         params,
                                         out_request_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        client->initialized = 1;
    }
    return status;
}

/* Provide the dap launch operation used by this module and its client applications. */
UmiStatus umi_dap_launch(UmiDapClient *client,
                         const char *program,
                         const char *working_directory,
                         int64_t *out_request_id)
{
    char escaped_program[UMI_PROTOCOL_URI_CAPACITY * 2U];
    char escaped_directory[UMI_PROTOCOL_URI_CAPACITY * 2U];
    char params[UMI_PROTOCOL_MESSAGE_CAPACITY];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (client == NULL || program == NULL || working_directory == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_json_escape(program,
                             escaped_program,
                             sizeof(escaped_program));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_json_escape(working_directory,
                                 escaped_directory,
                                 sizeof(escaped_directory));
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    (void)snprintf(params,
                   sizeof(params),
                   "{\"noDebug\":false,\"program\":\"%s\",\"cwd\":\"%s\"}",
                   escaped_program,
                   escaped_directory);
    return umi_protocol_client_request(client->client,
                                       "launch",
                                       params,
                                       out_request_id);
}

/* Provide the dap continue operation used by this module and its client applications. */
UmiStatus umi_dap_continue(UmiDapClient *client,
                           int thread_id,
                           int64_t *out_request_id)
{
    char params[256];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (client == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)snprintf(params,
                   sizeof(params),
                   "{\"threadId\":%d,\"singleThread\":false}",
                   thread_id);
    return umi_protocol_client_request(client->client,
                                       "continue",
                                       params,
                                       out_request_id);
}

/* Provide the dap disconnect operation used by this module and its client applications. */
UmiStatus umi_dap_disconnect(UmiDapClient *client,
                             int terminate_debuggee,
                             int64_t *out_request_id)
{
    char params[128];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (client == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)snprintf(params,
                   sizeof(params),
                   "{\"terminateDebuggee\":%s}",
                   terminate_debuggee ? "true" : "false");
    return umi_protocol_client_request(client->client,
                                       "disconnect",
                                       params,
                                       out_request_id);
}
