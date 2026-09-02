/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/quick_pick.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Quick Pick developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_QUICK_PICK_H
#define UMICOM_DEVELOPER_CAPABILITIES_QUICK_PICK_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_QUICK_PICK "umicom.developer.experience.quick_pick"

/**
 * Provide the developer capability quick pick operation used by this module and its client
 * applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_quick_pick(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_QUICK_PICK_H */
