/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/component.h
 *
 * PURPOSE:
 *   Describe one installed Framework SDK component and whether a consumer requires it.
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
#ifndef UMICOM_SDK_COMPONENT_H
#define UMICOM_SDK_COMPONENT_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkComponent { const char *component_id; const char *target_name; int required; } UmiSdkComponent;
UmiStatus umi_sdk_component_validate(const UmiSdkComponent *component);
#ifdef __cplusplus
}
#endif
#endif
