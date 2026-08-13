/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/profile_selection.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Profile Selection developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_PROFILE_SELECTION_H
#define UMICOM_DEVELOPER_CAPABILITIES_PROFILE_SELECTION_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_PROFILE_SELECTION "umicom.developer.configuration.profile_selection"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_profile_selection(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_PROFILE_SELECTION_H */
