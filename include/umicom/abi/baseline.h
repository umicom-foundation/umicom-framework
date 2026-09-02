/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/abi/baseline.h
 *
 * PURPOSE:
 *   Capture one expected Framework ABI baseline used by SDK and release-candidate checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * ABI files describe agreements between separately compiled code. They help us detect unsafe binary changes before an application is released.
 */
#ifndef UMICOM_ABI_BASELINE_H
#define UMICOM_ABI_BASELINE_H
#include <stdint.h>
#include "umicom/base/version.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the abi baseline data shared with callers of this public contract.
 */
typedef struct UmiAbiBaseline { UmiVersion framework_version; uint32_t framework_abi; uint64_t contract_fingerprint; } UmiAbiBaseline;
/**
 * Provide the abi baseline current operation used by this module and its client
 * applications.
 */
UmiAbiBaseline umi_abi_baseline_current(uint64_t fingerprint);
/**
 * Provide the abi baseline matches operation used by this module and its client
 * applications.
 */
int umi_abi_baseline_matches(const UmiAbiBaseline *expected, const UmiAbiBaseline *actual);
#ifdef __cplusplus
}
#endif
#endif
