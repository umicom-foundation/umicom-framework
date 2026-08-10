/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/protocol/dap.h
 *
 * PURPOSE:
 *   Create Debug Adapter Protocol initialise, launch, continue and disconnect requests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROTOCOL_DAP_H
#define UMICOM_PROTOCOL_DAP_H

#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/protocol/client.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDapClient {
    UmiProtocolClient *client;
    int initialized;
} UmiDapClient;

UmiStatus umi_dap_client_init(UmiDapClient *client,
                              UmiProtocolClient *protocol_client);
UmiStatus umi_dap_initialize(UmiDapClient *client,
                             const char *adapter_id,
                             int64_t *out_request_id);
UmiStatus umi_dap_launch(UmiDapClient *client,
                         const char *program,
                         const char *working_directory,
                         int64_t *out_request_id);
UmiStatus umi_dap_continue(UmiDapClient *client,
                           int thread_id,
                           int64_t *out_request_id);
UmiStatus umi_dap_disconnect(UmiDapClient *client,
                             int terminate_debuggee,
                             int64_t *out_request_id);

#ifdef __cplusplus
}
#endif

#endif
