/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/tool_probe.h
 *
 * PURPOSE:
 *   Discover and validate one requested native executable without probing a compiler.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef INCLUDE_UMICOM_TOOLCHAIN_TOOL_PROBE_H
#define INCLUDE_UMICOM_TOOLCHAIN_TOOL_PROBE_H
#include "umicom/diagnostics/diagnostic.h"
#include "umicom/toolchain/probe_report.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiToolchainToolProbeRequest {
    UmiToolKind kind;
    const char *explicit_root;
    int validate_version;
    UmiDiagnosticSink diagnostic_sink;
    void *diagnostic_user_data;
} UmiToolchainToolProbeRequest;

UmiStatus umi_toolchain_tool_probe(const UmiToolchainToolProbeRequest *request,
                                   UmiToolInfo *out_tool,
                                   UmiToolchainProbeReport *out_report);

#ifdef __cplusplus
}
#endif
#endif
