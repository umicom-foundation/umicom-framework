/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/go_to_offset_command.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Go To Offset Command document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_GO_TO_OFFSET_COMMAND_H
#define UMICOM_DOCUMENT_CAPABILITIES_GO_TO_OFFSET_COMMAND_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_GO_TO_OFFSET_COMMAND "umicom.document.navigation.go_to_offset_command"

/**
 * Provide the document capability go to offset command operation used by this module and
 * its client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_go_to_offset_command(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_GO_TO_OFFSET_COMMAND_H */
