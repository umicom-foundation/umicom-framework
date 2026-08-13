/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/windows_ucrt.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Windows UCRT developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_WINDOWS_UCRT_H
#define UMICOM_DEVELOPER_CAPABILITIES_WINDOWS_UCRT_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_WINDOWS_UCRT "umicom.developer.portability.windows_ucrt"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_windows_ucrt(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_WINDOWS_UCRT_H */
