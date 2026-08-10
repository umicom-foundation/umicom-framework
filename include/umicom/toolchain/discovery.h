/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/discovery.h
 *
 * PURPOSE:
 *   Discover compatible native compilers and development tools, validate their
 *   versions, and reject mixed or incomplete environments before a build begins.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiToolchainDiscoveryRequest {
    const char *explicit_root;
    const char *preferred_profile;
    int require_gtk;
    int require_java;
    int require_github_cli;
    UmiDiagnosticSink diagnostic_sink;
    void *diagnostic_user_data;
} UmiToolchainDiscoveryRequest;

typedef struct UmiToolchainDiscoveryReport {
    UmiToolchainProfile profile;
    size_t tools_found;
    size_t required_tools;
    size_t required_tools_missing;
    int compile_probe_passed;
    int link_probe_passed;
    int runtime_probe_passed;
} UmiToolchainDiscoveryReport;

UmiStatus umi_toolchain_discover(
    const UmiToolchainDiscoveryRequest *request,
    UmiToolchainDiscoveryReport *out_report
);
UmiStatus umi_toolchain_find_on_path(const char *executable,
                                     char *out_path,
                                     size_t capacity);
UmiStatus umi_toolchain_compile_probe(
    const UmiToolchainProfile *profile,
    UmiToolchainDiscoveryReport *in_out_report
);

#ifdef __cplusplus
}
#endif

#endif
