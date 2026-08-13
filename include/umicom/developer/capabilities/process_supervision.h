/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/process_supervision.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Process Supervision developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_PROCESS_SUPERVISION_H
#define UMICOM_DEVELOPER_CAPABILITIES_PROCESS_SUPERVISION_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_PROCESS_SUPERVISION "umicom.developer.execution.process_supervision"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_process_supervision(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_PROCESS_SUPERVISION_H */
