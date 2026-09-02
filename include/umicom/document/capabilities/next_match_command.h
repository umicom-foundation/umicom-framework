/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/next_match_command.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Next Match Command document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_NEXT_MATCH_COMMAND_H
#define UMICOM_DOCUMENT_CAPABILITIES_NEXT_MATCH_COMMAND_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_NEXT_MATCH_COMMAND "umicom.document.navigation.next_match_command"

/**
 * Provide the document capability next match command operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_next_match_command(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_NEXT_MATCH_COMMAND_H */
