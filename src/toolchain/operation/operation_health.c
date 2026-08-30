/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/operation/operation_health.c
 *
 * PURPOSE:
 *   Implement toolchain operation health projection.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/operation_health.h"

#include <string.h>

UmiStatus umi_toolchain_operation_health_from_context(
    const UmiToolchainOperationContext *context,
    UmiToolchainOperationHealth *out_health)
{
    if (context == NULL || out_health == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_health, 0, sizeof(*out_health));
    out_health->required_tools = context->discovery.requirement_count +
        (context->operation.requires_compiler ? 1U : 0U);
    out_health->found_tools = context->discovery.tools_found;
    out_health->missing_tools = context->discovery.required_missing;
    out_health->compiler_required = context->operation.requires_compiler;
    out_health->compile_probe_required = context->operation.run_compile_probe;
    out_health->healthy = context->ready && context->discovery.complete;
    return UMI_STATUS_OK;
}
