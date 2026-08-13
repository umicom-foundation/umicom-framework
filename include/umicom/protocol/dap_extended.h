/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/protocol/dap_extended.h
 * PURPOSE: Complete the provider-neutral Debug Adapter Protocol command set.
 * Created by: Sammy Hegab | Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROTOCOL_DAP_EXTENDED_H
#define UMICOM_PROTOCOL_DAP_EXTENDED_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/protocol/dap.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDapSourceBreakpoint {
    int line;
    int column;
    const char *condition;
    const char *hit_condition;
    const char *log_message;
} UmiDapSourceBreakpoint;

UmiStatus umi_dap_attach(UmiDapClient *client, int process_id,
                         const char *working_directory,
                         int64_t *out_request_id);
UmiStatus umi_dap_configuration_done(UmiDapClient *client,
                                     int64_t *out_request_id);
UmiStatus umi_dap_set_breakpoints(UmiDapClient *client,
                                  const char *source_path,
                                  const UmiDapSourceBreakpoint *items,
                                  size_t count,
                                  int64_t *out_request_id);
UmiStatus umi_dap_pause(UmiDapClient *client, int thread_id,
                        int64_t *out_request_id);
UmiStatus umi_dap_next(UmiDapClient *client, int thread_id,
                       int64_t *out_request_id);
UmiStatus umi_dap_step_in(UmiDapClient *client, int thread_id,
                          int64_t *out_request_id);
UmiStatus umi_dap_step_out(UmiDapClient *client, int thread_id,
                           int64_t *out_request_id);
UmiStatus umi_dap_threads(UmiDapClient *client, int64_t *out_request_id);
UmiStatus umi_dap_stack_trace(UmiDapClient *client, int thread_id,
                              int start_frame, int levels,
                              int64_t *out_request_id);
UmiStatus umi_dap_scopes(UmiDapClient *client, int frame_id,
                         int64_t *out_request_id);
UmiStatus umi_dap_variables(UmiDapClient *client, int variables_reference,
                            int start, int count,
                            int64_t *out_request_id);
UmiStatus umi_dap_evaluate(UmiDapClient *client, const char *expression,
                           int frame_id, const char *context,
                           int64_t *out_request_id);
UmiStatus umi_dap_set_variable(UmiDapClient *client, int variables_reference,
                               const char *name, const char *value,
                               int64_t *out_request_id);
UmiStatus umi_dap_restart(UmiDapClient *client, int64_t *out_request_id);
UmiStatus umi_dap_terminate(UmiDapClient *client, int restart,
                            int64_t *out_request_id);
UmiStatus umi_dap_source(UmiDapClient *client, int source_reference,
                         int64_t *out_request_id);
UmiStatus umi_dap_modules(UmiDapClient *client, int start, int count,
                          int64_t *out_request_id);
UmiStatus umi_dap_exception_info(UmiDapClient *client, int thread_id,
                                 int64_t *out_request_id);
UmiStatus umi_dap_loaded_sources(UmiDapClient *client,
                                 int64_t *out_request_id);
UmiStatus umi_dap_read_memory(UmiDapClient *client,
                              const char *memory_reference,
                              int offset, int count,
                              int64_t *out_request_id);
UmiStatus umi_dap_disassemble(UmiDapClient *client,
                              const char *memory_reference,
                              int offset, int instruction_count,
                              int64_t *out_request_id);

#ifdef __cplusplus
}
#endif
#endif
