/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/discovery.h
 *
 * PURPOSE:
 *   Discover compatible native compilers and development tools, validate their
 *   versions, and reject mixed or incomplete environments before a build begins.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_DISCOVERY_H
#define UMICOM_TOOLCHAIN_DISCOVERY_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/diagnostics/diagnostic.h"
#include "umicom/toolchain/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the toolchain discovery request data shared with callers of this public
 * contract.
 */
typedef struct UmiToolchainDiscoveryRequest {
    const char *explicit_root;
    const char *preferred_profile;
    int require_gtk;
    int require_java;
    int require_github_cli;
    int skip_compile_probe;
    UmiDiagnosticSink diagnostic_sink;
    void *diagnostic_user_data;
} UmiToolchainDiscoveryRequest;

/**
 * Represent the toolchain discovery report data shared with callers of this public
 * contract.
 */
typedef struct UmiToolchainDiscoveryReport {
    UmiToolchainProfile profile;
    size_t tools_found;
    size_t required_tools;
    size_t required_tools_missing;
    int compile_probe_passed;
    int link_probe_passed;
    int runtime_probe_passed;
    int c23_probe_passed;
} UmiToolchainDiscoveryReport;

/**
 * Initialise toolchain discovery request from caller-provided values so later operations
 * receive a known state.
 */
void umi_toolchain_discovery_request_init(
    UmiToolchainDiscoveryRequest *request);

/**
 * Provide the toolchain discover operation used by this module and its client
 * applications.
 */
UmiStatus umi_toolchain_discover(
    const UmiToolchainDiscoveryRequest *request,
    UmiToolchainDiscoveryReport *out_report);

/**
 * Provide the toolchain find on path operation used by this module and its client
 * applications.
 */
UmiStatus umi_toolchain_find_on_path(const char *executable,
                                     char *out_path,
                                     size_t capacity);
/**
 * Provide the toolchain compile probe operation used by this module and its client
 * applications.
 */
UmiStatus umi_toolchain_compile_probe(
    const UmiToolchainProfile *profile,
    UmiToolchainDiscoveryReport *in_out_report);

#ifdef __cplusplus
}
#endif

#endif
