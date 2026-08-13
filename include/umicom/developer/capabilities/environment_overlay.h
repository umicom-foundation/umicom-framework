/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/environment_overlay.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Environment Overlay developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_ENVIRONMENT_OVERLAY_H
#define UMICOM_DEVELOPER_CAPABILITIES_ENVIRONMENT_OVERLAY_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_ENVIRONMENT_OVERLAY "umicom.developer.execution.environment_overlay"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_environment_overlay(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_ENVIRONMENT_OVERLAY_H */
