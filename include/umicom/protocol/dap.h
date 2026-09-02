/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/protocol/dap.h
 *
 * PURPOSE:
 *   Create Debug Adapter Protocol initialise, launch, continue and disconnect requests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROTOCOL_DAP_H
#define UMICOM_PROTOCOL_DAP_H

#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/protocol/client.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dap client data shared with callers of this public contract.
 */
typedef struct UmiDapClient {
    UmiProtocolClient *client;
    int initialized;
} UmiDapClient;

/**
 * Initialise dap client from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_dap_client_init(UmiDapClient *client,
                              UmiProtocolClient *protocol_client);
/**
 * Initialise dap from caller-provided values so later operations receive a known state.
 */
UmiStatus umi_dap_initialize(UmiDapClient *client,
                             const char *adapter_id,
                             int64_t *out_request_id);
/**
 * Provide the dap launch operation used by this module and its client applications.
 */
UmiStatus umi_dap_launch(UmiDapClient *client,
                         const char *program,
                         const char *working_directory,
                         int64_t *out_request_id);
/**
 * Provide the dap continue operation used by this module and its client applications.
 */
UmiStatus umi_dap_continue(UmiDapClient *client,
                           int thread_id,
                           int64_t *out_request_id);
/**
 * Provide the dap disconnect operation used by this module and its client applications.
 */
UmiStatus umi_dap_disconnect(UmiDapClient *client,
                             int terminate_debuggee,
                             int64_t *out_request_id);

#ifdef __cplusplus
}
#endif

#endif
