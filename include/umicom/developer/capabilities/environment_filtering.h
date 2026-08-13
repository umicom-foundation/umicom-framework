/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/environment_filtering.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Environment Filtering developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_ENVIRONMENT_FILTERING_H
#define UMICOM_DEVELOPER_CAPABILITIES_ENVIRONMENT_FILTERING_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_ENVIRONMENT_FILTERING "umicom.developer.security.environment_filtering"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_environment_filtering(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_ENVIRONMENT_FILTERING_H */
