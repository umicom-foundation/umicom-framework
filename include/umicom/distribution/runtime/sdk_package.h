/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/sdk_package.h
 *
 * PURPOSE:
 *   developer SDK package metadata and ABI compatibility range.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_SDK_PACKAGE_H
#define UMICOM_DISTRIBUTION_RUNTIME_SDK_PACKAGE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr sdk package data shared with callers of this public contract.
 */
typedef struct UmiDrSdkPackage { char id[UMI_DR_ID_CAPACITY]; UmiDrVersion version; UmiDrVersion minimum_abi; UmiDrVersion maximum_abi; bool headers; bool libraries; } UmiDrSdkPackage;
/**
 * Initialise dr sdk package from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_sdk_package_init(UmiDrSdkPackage *value);
/**
 * Check that dr sdk package satisfies its contract before another service relies on it.
 */
bool umi_dr_sdk_package_valid(const UmiDrSdkPackage *value);
/**
 * Provide the dr sdk package fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_sdk_package_fingerprint(const UmiDrSdkPackage *value);

#ifdef __cplusplus
}
#endif
#endif
