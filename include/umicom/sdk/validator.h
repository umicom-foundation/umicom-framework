/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/validator.h
 *
 * PURPOSE:
 *   Run a complete SDK-prefix validation using package, header and ABI evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * SDK APIs verify that an application can consume an installed Umicom Framework package without depending on private source-tree details.
 */
#ifndef UMICOM_SDK_VALIDATOR_H
#define UMICOM_SDK_VALIDATOR_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/base/version.h"
#include "umicom/sdk/probe.h"
#include "umicom/sdk/compatibility.h"
#include "umicom/sdk/conformance.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkValidationResult { UmiSdkProbeResult probe; UmiSdkConformance conformance; int compatible; int passed; } UmiSdkValidationResult;
UmiStatus umi_sdk_validate_prefix(const char *prefix,const UmiSdkRequirement *requirement,UmiSdkValidationResult *out_result);
#ifdef __cplusplus
}
#endif
#endif
