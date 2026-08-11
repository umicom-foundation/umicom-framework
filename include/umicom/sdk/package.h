/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/package.h
 *
 * PURPOSE:
 *   Represent the CMake package files expected from an installed SDK.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * SDK APIs verify that an application can consume an installed Umicom Framework package without depending on private source-tree details.
 */
#ifndef UMICOM_SDK_PACKAGE_H
#define UMICOM_SDK_PACKAGE_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_SDK_PATH_CAPACITY 1024U
typedef struct UmiSdkPackage { char prefix[UMI_SDK_PATH_CAPACITY]; char config_file[UMI_SDK_PATH_CAPACITY]; char version_file[UMI_SDK_PATH_CAPACITY]; char targets_file[UMI_SDK_PATH_CAPACITY]; } UmiSdkPackage;
UmiStatus umi_sdk_package_from_prefix(const char *prefix,UmiSdkPackage *out_package);
#ifdef __cplusplus
}
#endif
#endif
