/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/working_directory_sync.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Working Directory Sync developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_WORKING_DIRECTORY_SYNC_H
#define UMICOM_DEVELOPER_CAPABILITIES_WORKING_DIRECTORY_SYNC_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_WORKING_DIRECTORY_SYNC "umicom.developer.terminal.working_directory_sync"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_working_directory_sync(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_WORKING_DIRECTORY_SYNC_H */
