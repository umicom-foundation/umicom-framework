/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/update_feed.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Update Feed developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_UPDATE_FEED_H
#define UMICOM_DEVELOPER_CAPABILITIES_UPDATE_FEED_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_UPDATE_FEED "umicom.developer.delivery.update_feed"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_update_feed(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_UPDATE_FEED_H */
