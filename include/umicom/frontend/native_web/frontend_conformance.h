/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/frontend_conformance.h
 *
 * PURPOSE:
 *   Evaluate semantic capability parity between native-web and other Framework frontend adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_FRONTEND_CONFORMANCE_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_FRONTEND_CONFORMANCE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/browser_capability.h"
typedef struct UmiNativeWebConformanceResult { uint64_t required_flags; uint64_t provided_flags; uint64_t missing_flags; unsigned score_percent; bool conformant; } UmiNativeWebConformanceResult;
/* Evaluate native-web capability parity against a semantic frontend requirement mask. */
UmiStatus umi_native_web_frontend_conformance(uint64_t required_flags, const UmiNativeWebBrowserCapability *provided, UmiNativeWebConformanceResult *out_result);

#ifdef __cplusplus
}
#endif
#endif
