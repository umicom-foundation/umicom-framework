/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/notifications.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Notifications developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_NOTIFICATIONS_H
#define UMICOM_DEVELOPER_CAPABILITIES_NOTIFICATIONS_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_NOTIFICATIONS "umicom.developer.experience.notifications"

/**
 * Provide the developer capability notifications operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_notifications(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_NOTIFICATIONS_H */
