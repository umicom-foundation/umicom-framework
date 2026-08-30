/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/command_palette.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Command Palette developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_COMMAND_PALETTE_H
#define UMICOM_DEVELOPER_CAPABILITIES_COMMAND_PALETTE_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_COMMAND_PALETTE "umicom.developer.experience.command_palette"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_command_palette(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_COMMAND_PALETTE_H */
