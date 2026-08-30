/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/operation_context.h
 *
 * PURPOSE:
 *   Compose an operation profile, scoped discovery report and minimal child environment.
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
#ifndef INCLUDE_UMICOM_TOOLCHAIN_OPERATION_CONTEXT_H
#define INCLUDE_UMICOM_TOOLCHAIN_OPERATION_CONTEXT_H
#include "umicom/toolchain/operation_catalogue.h"
#include "umicom/toolchain/scoped_discovery.h"
#include "umicom/toolchain/scoped_environment.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiToolchainOperationContext {
    UmiToolchainOperationProfile operation;
    UmiToolchainScopedDiscoveryReport discovery;
    UmiEnvironmentPlan environment;
    int ready;
} UmiToolchainOperationContext;

UmiStatus umi_toolchain_operation_context_prepare(
    UmiToolchainOperationKind kind,
    const char *explicit_toolchain_root,
    const char *preferred_profile,
    UmiDiagnosticSink diagnostic_sink,
    void *diagnostic_user_data,
    UmiToolchainOperationContext *out_context);

#ifdef __cplusplus
}
#endif
#endif
