/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/terminal_transcript.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Terminal Transcript developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_TRANSCRIPT_H
#define UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_TRANSCRIPT_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_TERMINAL_TRANSCRIPT "umicom.developer.terminal.terminal_transcript"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_terminal_transcript(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_TRANSCRIPT_H */
