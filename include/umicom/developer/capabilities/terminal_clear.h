/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/terminal_clear.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Terminal Clear developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_CLEAR_H
#define UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_CLEAR_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_TERMINAL_CLEAR "umicom.developer.terminal.terminal_clear"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_terminal_clear(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_CLEAR_H */
