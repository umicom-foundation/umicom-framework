/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/attach_process.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Attach Process developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_ATTACH_PROCESS_H
#define UMICOM_DEVELOPER_CAPABILITIES_ATTACH_PROCESS_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_ATTACH_PROCESS "umicom.developer.execution.attach_process"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_attach_process(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_ATTACH_PROCESS_H */
