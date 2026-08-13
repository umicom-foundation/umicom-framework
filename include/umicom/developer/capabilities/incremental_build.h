/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/incremental_build.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Incremental Build developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_INCREMENTAL_BUILD_H
#define UMICOM_DEVELOPER_CAPABILITIES_INCREMENTAL_BUILD_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_INCREMENTAL_BUILD "umicom.developer.build.incremental_build"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_incremental_build(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_INCREMENTAL_BUILD_H */
