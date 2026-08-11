/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/target.h
 *
 * PURPOSE:
 *   Describe one exported CMake target expected by SDK consumers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * SDK APIs verify that an application can consume an installed Umicom Framework package without depending on private source-tree details.
 */
#ifndef UMICOM_SDK_TARGET_H
#define UMICOM_SDK_TARGET_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkTarget { const char *target_name; const char *component_id; int public_target; } UmiSdkTarget;
UmiStatus umi_sdk_target_validate(const UmiSdkTarget *target);
#ifdef __cplusplus
}
#endif
#endif
