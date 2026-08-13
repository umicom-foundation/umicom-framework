/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/task_status.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Task Status developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_TASK_STATUS_H
#define UMICOM_DEVELOPER_CAPABILITIES_TASK_STATUS_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_TASK_STATUS "umicom.developer.experience.task_status"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_task_status(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_TASK_STATUS_H */
