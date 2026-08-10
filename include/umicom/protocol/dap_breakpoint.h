/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/protocol/dap_breakpoint.h
 *
 * PURPOSE:
 *   Maintain source breakpoints and generate Debug Adapter setBreakpoints requests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiDapBreakpoint {
    char source_path[UMI_PROTOCOL_URI_CAPACITY];
    int line;
    int column;
    int enabled;
    int verified;
} UmiDapBreakpoint;

typedef struct UmiDapBreakpointRegistry UmiDapBreakpointRegistry;

UmiStatus umi_dap_breakpoint_registry_create(
    UmiDapBreakpointRegistry **out_registry
);
void umi_dap_breakpoint_registry_destroy(
    UmiDapBreakpointRegistry *registry
);
UmiStatus umi_dap_breakpoint_add(UmiDapBreakpointRegistry *registry,
                                 const UmiDapBreakpoint *breakpoint);
UmiStatus umi_dap_breakpoint_remove(UmiDapBreakpointRegistry *registry,
                                    const char *source_path,
                                    int line);
size_t umi_dap_breakpoint_count(const UmiDapBreakpointRegistry *registry);
const UmiDapBreakpoint *umi_dap_breakpoint_at(
    const UmiDapBreakpointRegistry *registry,
    size_t index
);
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
