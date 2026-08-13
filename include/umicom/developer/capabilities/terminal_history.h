/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/terminal_history.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Terminal History developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_HISTORY_H
#define UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_HISTORY_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_TERMINAL_HISTORY "umicom.developer.terminal.terminal_history"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_terminal_history(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_HISTORY_H */
