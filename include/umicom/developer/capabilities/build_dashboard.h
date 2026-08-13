/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/build_dashboard.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Build Dashboard developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_BUILD_DASHBOARD_H
#define UMICOM_DEVELOPER_CAPABILITIES_BUILD_DASHBOARD_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_BUILD_DASHBOARD "umicom.developer.experience.build_dashboard"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_build_dashboard(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_BUILD_DASHBOARD_H */
