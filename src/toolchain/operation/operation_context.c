/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/operation/operation_context.c
 *
 * PURPOSE:
 *   Prepare operation contexts while keeping repository commands independent of compiler health.
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
#include "umicom/toolchain/operation_context.h"

#include <string.h>

/*
 * Provide the toolchain operation context prepare operation used by this module and its
 * client applications.
 */
UmiStatus umi_toolchain_operation_context_prepare(
    UmiToolchainOperationKind kind,
    const char *explicit_toolchain_root,
    const char *preferred_profile,
    UmiDiagnosticSink diagnostic_sink,
    void *diagnostic_user_data,
    UmiToolchainOperationContext *out_context)
{
    UmiToolchainScopedDiscoveryRequest request;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_context, 0, sizeof(*out_context));
    status = umi_toolchain_operation_catalogue_profile(kind, &out_context->operation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)memset(&request, 0, sizeof(request));
    request.operation = &out_context->operation;
    request.explicit_root = explicit_toolchain_root;
    request.preferred_profile = preferred_profile;
    request.diagnostic_sink = diagnostic_sink;
    request.diagnostic_user_data = diagnostic_user_data;
    status = umi_toolchain_discover_scoped(&request, &out_context->discovery);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_toolchain_scoped_environment(
        &out_context->discovery.profile, &out_context->operation, &out_context->environment);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_context->ready = 1;
    return UMI_STATUS_OK;
}
