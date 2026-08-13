/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/macos.h
 *
 * PURPOSE:
 *   Declare the Framework-owned macOS developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_MACOS_H
#define UMICOM_DEVELOPER_CAPABILITIES_MACOS_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_MACOS "umicom.developer.portability.macos"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_macos(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_MACOS_H */
