/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/profile.h
 *
 * PURPOSE:
 *   Describe an SDK build profile such as static, shared, debug or release.
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
#ifndef UMICOM_SDK_PROFILE_H
#define UMICOM_SDK_PROFILE_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named sdk linkage values accepted by this public contract.
 */
typedef enum UmiSdkLinkage { UMI_SDK_LINKAGE_STATIC=1, UMI_SDK_LINKAGE_SHARED=2 } UmiSdkLinkage;
/**
 * Represent the sdk profile data shared with callers of this public contract.
 */
typedef struct UmiSdkProfile { const char *profile_id; UmiSdkLinkage linkage; int debug; int strict_warnings; } UmiSdkProfile;
/**
 * Check that sdk profile satisfies its contract before another service relies on it.
 */
UmiStatus umi_sdk_profile_validate(const UmiSdkProfile *profile);
#ifdef __cplusplus
}
#endif
#endif
