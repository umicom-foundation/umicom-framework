/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/dependency.h
 *
 * PURPOSE:
 *   Describe one external dependency required by an exported Framework target.
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
#ifndef UMICOM_SDK_DEPENDENCY_H
#define UMICOM_SDK_DEPENDENCY_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkDependency { const char *package_name; const char *minimum_version; int required; } UmiSdkDependency;
UmiStatus umi_sdk_dependency_validate(const UmiSdkDependency *dependency);
#ifdef __cplusplus
}
#endif
#endif
