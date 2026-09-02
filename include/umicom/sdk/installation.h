/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/installation.h
 *
 * PURPOSE:
 *   Describe an installed SDK prefix and its include, library and CMake directories.
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
#ifndef UMICOM_SDK_INSTALLATION_H
#define UMICOM_SDK_INSTALLATION_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk installation data shared with callers of this public contract.
 */
typedef struct UmiSdkInstallation { char prefix[1024]; char include_dir[1024]; char library_dir[1024]; char cmake_dir[1024]; } UmiSdkInstallation;
/**
 * Provide the sdk installation from prefix operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_installation_from_prefix(const char *prefix,UmiSdkInstallation *out_installation);
#ifdef __cplusplus
}
#endif
#endif
