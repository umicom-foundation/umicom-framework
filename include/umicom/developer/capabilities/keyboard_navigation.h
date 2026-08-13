/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/keyboard_navigation.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Keyboard Navigation developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_KEYBOARD_NAVIGATION_H
#define UMICOM_DEVELOPER_CAPABILITIES_KEYBOARD_NAVIGATION_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_KEYBOARD_NAVIGATION "umicom.developer.experience.keyboard_navigation"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_keyboard_navigation(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_KEYBOARD_NAVIGATION_H */
