/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/operation_service.h
 *
 * PURPOSE:
 *   Expose a reusable stateful service for native operation preparation and health inspection.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef INCLUDE_UMICOM_TOOLCHAIN_OPERATION_SERVICE_H
#define INCLUDE_UMICOM_TOOLCHAIN_OPERATION_SERVICE_H
#include <stdint.h>
#include "umicom/toolchain/operation_context.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiToolchainOperationServiceState {
    UMI_TOOLCHAIN_OPERATION_SERVICE_IDLE = 0,
    UMI_TOOLCHAIN_OPERATION_SERVICE_PREPARING = 1,
    UMI_TOOLCHAIN_OPERATION_SERVICE_READY = 2,
    UMI_TOOLCHAIN_OPERATION_SERVICE_FAILED = 3
} UmiToolchainOperationServiceState;

typedef struct UmiToolchainOperationService {
    UmiToolchainOperationContext context;
    UmiToolchainOperationServiceState state;
    UmiStatus last_status;
    uint64_t revision;
} UmiToolchainOperationService;

void umi_toolchain_operation_service_init(UmiToolchainOperationService *service);
UmiStatus umi_toolchain_operation_service_prepare(
    UmiToolchainOperationService *service,
    UmiToolchainOperationKind kind,
    const char *explicit_toolchain_root,
    const char *preferred_profile,
    UmiDiagnosticSink diagnostic_sink,
    void *diagnostic_user_data);
int umi_toolchain_operation_service_ready(const UmiToolchainOperationService *service);

#ifdef __cplusplus
}
#endif
#endif
