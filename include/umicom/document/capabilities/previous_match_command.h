/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/previous_match_command.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Previous Match Command document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_PREVIOUS_MATCH_COMMAND_H
#define UMICOM_DOCUMENT_CAPABILITIES_PREVIOUS_MATCH_COMMAND_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_PREVIOUS_MATCH_COMMAND "umicom.document.navigation.previous_match_command"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_previous_match_command(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_PREVIOUS_MATCH_COMMAND_H */
