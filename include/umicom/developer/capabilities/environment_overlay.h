/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/environment_overlay.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Environment Overlay developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_ENVIRONMENT_OVERLAY_H
#define UMICOM_DEVELOPER_CAPABILITIES_ENVIRONMENT_OVERLAY_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_ENVIRONMENT_OVERLAY "umicom.developer.execution.environment_overlay"

/**
 * Provide the developer capability environment overlay operation used by this module and
 * its client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_environment_overlay(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_ENVIRONMENT_OVERLAY_H */
