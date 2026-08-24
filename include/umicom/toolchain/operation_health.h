/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/operation_health.h
 *
 * PURPOSE:
 *   Project scoped discovery state into lightweight health information for UI and diagnostics.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef INCLUDE_UMICOM_TOOLCHAIN_OPERATION_HEALTH_H
#define INCLUDE_UMICOM_TOOLCHAIN_OPERATION_HEALTH_H
#include "umicom/toolchain/operation_context.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiToolchainOperationHealth {
    size_t required_tools;
    size_t found_tools;
    size_t missing_tools;
    int compiler_required;
    int compile_probe_required;
    int healthy;
} UmiToolchainOperationHealth;

UmiStatus umi_toolchain_operation_health_from_context(
    const UmiToolchainOperationContext *context,
    UmiToolchainOperationHealth *out_health);

#ifdef __cplusplus
}
#endif
#endif
