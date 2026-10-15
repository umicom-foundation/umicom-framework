/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/operation/operation_service.c
 *
 * PURPOSE:
 *   Implement stateful operation-context preparation for CLI and application consumers.
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
#include <stdint.h>
#include "umicom/toolchain/operation_service.h"

#include <string.h>

/*
 * Initialise toolchain operation service from caller-provided values so later operations
 * receive a known state.
 */
void umi_toolchain_operation_service_init(UmiToolchainOperationService *service)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return;
    (void)memset(service, 0, sizeof(*service));
    service->state = UMI_TOOLCHAIN_OPERATION_SERVICE_IDLE;
    service->last_status = UMI_STATUS_OK;
    service->revision = 1U;
}

/*
 * Provide the toolchain operation service prepare operation used by this module and its
 * client applications.
 */
/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_toolchain_operation_service_prepare(
    UmiToolchainOperationService *service,
    UmiToolchainOperationKind kind,
    const char *explicit_toolchain_root,
    const char *preferred_profile,
    UmiDiagnosticSink diagnostic_sink,
    void *diagnostic_user_data)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    service->state = UMI_TOOLCHAIN_OPERATION_SERVICE_PREPARING;
    status = umi_toolchain_operation_context_prepare(
        kind, explicit_toolchain_root, preferred_profile,
        diagnostic_sink, diagnostic_user_data, &service->context);
    service->last_status = status;
    service->state = status == UMI_STATUS_OK
        ? UMI_TOOLCHAIN_OPERATION_SERVICE_READY
        : UMI_TOOLCHAIN_OPERATION_SERVICE_FAILED;
    service->revision += 1U;
    return status;
}
#endif

UmiStatus umi_toolchain_operation_service_prepare(
    UmiToolchainOperationService *service,
    UmiToolchainOperationKind kind,
    const char *explicit_toolchain_root,
    const char *preferred_profile,
    UmiDiagnosticSink diagnostic_sink,
    void *diagnostic_user_data)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (service != NULL && service->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    service->state = UMI_TOOLCHAIN_OPERATION_SERVICE_PREPARING;
    status = umi_toolchain_operation_context_prepare(
        kind, explicit_toolchain_root, preferred_profile,
        diagnostic_sink, diagnostic_user_data, &service->context);
    service->last_status = status;
    service->state = status == UMI_STATUS_OK
        ? UMI_TOOLCHAIN_OPERATION_SERVICE_READY
        : UMI_TOOLCHAIN_OPERATION_SERVICE_FAILED;
    service->revision += 1U;
    return status;
}

/*
 * Provide the toolchain operation service ready operation used by this module and its
 * client applications.
 */
int umi_toolchain_operation_service_ready(const UmiToolchainOperationService *service)
{
    return service != NULL && service->state == UMI_TOOLCHAIN_OPERATION_SERVICE_READY &&
           service->context.ready;
}
