/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/toolchain_detection.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Toolchain Detection developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_TOOLCHAIN_DETECTION_H
#define UMICOM_DEVELOPER_CAPABILITIES_TOOLCHAIN_DETECTION_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_TOOLCHAIN_DETECTION "umicom.developer.configuration.toolchain_detection"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_toolchain_detection(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_TOOLCHAIN_DETECTION_H */
