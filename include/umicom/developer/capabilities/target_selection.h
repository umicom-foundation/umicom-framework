/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/target_selection.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Target Selection developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_TARGET_SELECTION_H
#define UMICOM_DEVELOPER_CAPABILITIES_TARGET_SELECTION_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_TARGET_SELECTION "umicom.developer.build.target_selection"

/**
 * Provide the developer capability target selection operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_target_selection(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_TARGET_SELECTION_H */
