/* Umicom Framework internal DAP helpers. Sammy Hegab, Umicom Foundation, MIT. */
#ifndef UMICOM_PROTOCOL_DAP_REQUEST_INTERNAL_H
#define UMICOM_PROTOCOL_DAP_REQUEST_INTERNAL_H
#include "umicom/protocol/dap_extended.h"
UmiStatus umi_dap_request(UmiDapClient *client, const char *command,
                          const char *arguments_json,
                          int64_t *out_request_id);
UmiStatus umi_dap_thread_request(UmiDapClient *client, const char *command,
                                 int thread_id, const char *suffix_json,
                                 int64_t *out_request_id);
UmiStatus umi_dap_escape(const char *text, char *out, size_t capacity);
#endif
