/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/abi/check.h
 *
 * PURPOSE:
 *   Run a compact release-candidate ABI check from descriptor, baseline and platform evidence.
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
#ifndef UMICOM_ABI_CHECK_H
#define UMICOM_ABI_CHECK_H
#include "umicom/abi/baseline.h"
#include "umicom/abi/descriptor.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the abi check result data shared with callers of this public contract.
 */
typedef struct UmiAbiCheckResult { int descriptor_valid; int platform_valid; int baseline_valid; int passed; } UmiAbiCheckResult;
/**
 * Perform abi check through the module contract so client applications do not duplicate
 * its policy.
 */
UmiStatus umi_abi_check_run(const UmiAbiDescriptor *descriptor, const UmiAbiBaseline *expected, const UmiAbiBaseline *actual, UmiAbiCheckResult *out_result);
#ifdef __cplusplus
}
#endif
#endif
