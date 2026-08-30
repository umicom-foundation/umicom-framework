/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/probe_report.h
 *
 * PURPOSE:
 *   Record one native executable discovery/validation result without losing diagnostics.
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
#ifndef INCLUDE_UMICOM_TOOLCHAIN_PROBE_REPORT_H
#define INCLUDE_UMICOM_TOOLCHAIN_PROBE_REPORT_H
#include "umicom/base/status.h"
#include "umicom/toolchain/tool.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TOOLCHAIN_PROBE_OUTPUT_CAPACITY 1024U

typedef struct UmiToolchainProbeReport {
    UmiToolKind kind;
    UmiStatus status;
    int found;
    int validated;
    int from_explicit_root;
    char path[UMI_TOOL_PATH_CAPACITY];
    char version[UMI_TOOL_VERSION_CAPACITY];
    char detail[UMI_TOOLCHAIN_PROBE_OUTPUT_CAPACITY];
} UmiToolchainProbeReport;

void umi_toolchain_probe_report_init(UmiToolchainProbeReport *report,
                                     UmiToolKind kind);
UmiStatus umi_toolchain_probe_report_validate(const UmiToolchainProbeReport *report);

#ifdef __cplusplus
}
#endif
#endif
