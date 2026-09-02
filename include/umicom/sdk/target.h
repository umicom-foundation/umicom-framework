/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/target.h
 *
 * PURPOSE:
 *   Describe one exported CMake target expected by SDK consumers.
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
#ifndef UMICOM_SDK_TARGET_H
#define UMICOM_SDK_TARGET_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk target data shared with callers of this public contract.
 */
typedef struct UmiSdkTarget { const char *target_name; const char *component_id; int public_target; } UmiSdkTarget;
/**
 * Check that sdk target satisfies its contract before another service relies on it.
 */
UmiStatus umi_sdk_target_validate(const UmiSdkTarget *target);
#ifdef __cplusplus
}
#endif
#endif
