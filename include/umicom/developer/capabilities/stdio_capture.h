/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/stdio_capture.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Standard IO Capture developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_STDIO_CAPTURE_H
#define UMICOM_DEVELOPER_CAPABILITIES_STDIO_CAPTURE_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_STDIO_CAPTURE "umicom.developer.execution.stdio_capture"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_stdio_capture(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_STDIO_CAPTURE_H */
