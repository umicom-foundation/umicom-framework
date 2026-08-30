/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/probe.h
 *
 * PURPOSE:
 *   Probe required public headers, libraries and CMake package files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * SDK APIs verify that an application can consume an installed Umicom Framework package without depending on private source-tree details.
 */
#ifndef UMICOM_SDK_PROBE_H
#define UMICOM_SDK_PROBE_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkProbeResult { int package_config; int public_header; int libraries; int passed; } UmiSdkProbeResult;
UmiStatus umi_sdk_probe_prefix(const char *prefix,UmiSdkProbeResult *out_result);
#ifdef __cplusplus
}
#endif
#endif
