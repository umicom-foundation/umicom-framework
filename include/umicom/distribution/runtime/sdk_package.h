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

typedef struct UmiDrSdkPackage { char id[UMI_DR_ID_CAPACITY]; UmiDrVersion version; UmiDrVersion minimum_abi; UmiDrVersion maximum_abi; bool headers; bool libraries; } UmiDrSdkPackage;
void umi_dr_sdk_package_init(UmiDrSdkPackage *value);
bool umi_dr_sdk_package_valid(const UmiDrSdkPackage *value);
uint64_t umi_dr_sdk_package_fingerprint(const UmiDrSdkPackage *value);

#ifdef __cplusplus
}
#endif
#endif
