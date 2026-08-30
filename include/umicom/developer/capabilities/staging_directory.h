/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/staging_directory.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Staging Directory developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_STAGING_DIRECTORY_H
#define UMICOM_DEVELOPER_CAPABILITIES_STAGING_DIRECTORY_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_STAGING_DIRECTORY "umicom.developer.delivery.staging_directory"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_staging_directory(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_STAGING_DIRECTORY_H */
