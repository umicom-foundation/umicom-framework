/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/protocol/dap_breakpoint.h
 *
 * PURPOSE:
 *   Maintain source breakpoints and generate Debug Adapter setBreakpoints requests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROTOCOL_DAP_BREAKPOINT_H
#define UMICOM_PROTOCOL_DAP_BREAKPOINT_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/protocol/client.h"
#include "umicom/protocol/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dap breakpoint data shared with callers of this public contract.
 */
typedef struct UmiDapBreakpoint {
    char source_path[UMI_PROTOCOL_URI_CAPACITY];
    int line;
    int column;
    int enabled;
    int verified;
} UmiDapBreakpoint;

/**
 * Represent the dap breakpoint registry data shared with callers of this public contract.
 */
typedef struct UmiDapBreakpointRegistry UmiDapBreakpointRegistry;

/**
 * Initialise dap breakpoint registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_dap_breakpoint_registry_create(
    UmiDapBreakpointRegistry **out_registry
);
/**
 * Release or reset state held by dap breakpoint registry so the same storage can be reused
 * safely.
 */
void umi_dap_breakpoint_registry_destroy(
    UmiDapBreakpointRegistry *registry
);
/**
 * Add dap breakpoint only after its inputs and available capacity have been checked.
 */
UmiStatus umi_dap_breakpoint_add(UmiDapBreakpointRegistry *registry,
                                 const UmiDapBreakpoint *breakpoint);
/**
 * Remove dap breakpoint while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_dap_breakpoint_remove(UmiDapBreakpointRegistry *registry,
                                    const char *source_path,
                                    int line);
/**
 * Return the number of records represented by dap breakpoint without changing their state.
 */
size_t umi_dap_breakpoint_count(const UmiDapBreakpointRegistry *registry);
/**
 * Find dap breakpoint while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiDapBreakpoint *umi_dap_breakpoint_at(
    const UmiDapBreakpointRegistry *registry,
    size_t index
);
/**
 * Provide the dap breakpoint build request operation used by this module and its client
 * applications.
 */
UmiStatus umi_dap_breakpoint_build_request(
    const UmiDapBreakpointRegistry *registry,
    UmiProtocolClient *client,
    const char *source_path,
    int64_t *out_request_id
);

#ifdef __cplusplus
}
#endif

#endif
