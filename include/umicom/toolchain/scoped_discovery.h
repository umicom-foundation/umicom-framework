/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/scoped_discovery.h
 *
 * PURPOSE:
 *   Discover only tools required by one native operation, avoiding unrelated compiler probes.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef INCLUDE_UMICOM_TOOLCHAIN_SCOPED_DISCOVERY_H
#define INCLUDE_UMICOM_TOOLCHAIN_SCOPED_DISCOVERY_H
#include "umicom/toolchain/operation_profile.h"
#include "umicom/toolchain/tool_probe.h"
#include "umicom/toolchain/profile.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiToolchainScopedDiscoveryRequest {
    const UmiToolchainOperationProfile *operation;
    const char *explicit_root;
    const char *preferred_profile;
    UmiDiagnosticSink diagnostic_sink;
    void *diagnostic_user_data;
} UmiToolchainScopedDiscoveryRequest;

typedef struct UmiToolchainScopedDiscoveryReport {
    UmiToolchainProfile profile;
    size_t requirement_count;
    size_t tools_found;
    size_t required_missing;
    int compiler_resolved;
    int compile_probe_passed;
    int complete;
} UmiToolchainScopedDiscoveryReport;

UmiStatus umi_toolchain_discover_scoped(
    const UmiToolchainScopedDiscoveryRequest *request,
    UmiToolchainScopedDiscoveryReport *out_report);

#ifdef __cplusplus
}
#endif
#endif
