/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/watch_debounce.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Watch Debounce developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_WATCH_DEBOUNCE_H
#define UMICOM_DEVELOPER_CAPABILITIES_WATCH_DEBOUNCE_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_WATCH_DEBOUNCE "umicom.developer.performance.watch_debounce"

/**
 * Provide the developer capability watch debounce operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_watch_debounce(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_WATCH_DEBOUNCE_H */
