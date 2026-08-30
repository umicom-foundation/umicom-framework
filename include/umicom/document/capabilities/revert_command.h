/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/revert_command.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Revert Command document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_REVERT_COMMAND_H
#define UMICOM_DOCUMENT_CAPABILITIES_REVERT_COMMAND_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_REVERT_COMMAND "umicom.document.command.revert_command"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_revert_command(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_REVERT_COMMAND_H */
