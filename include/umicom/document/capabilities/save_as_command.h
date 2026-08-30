/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/save_as_command.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Save As Command document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_SAVE_AS_COMMAND_H
#define UMICOM_DOCUMENT_CAPABILITIES_SAVE_AS_COMMAND_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_SAVE_AS_COMMAND "umicom.document.command.save_as_command"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_save_as_command(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_SAVE_AS_COMMAND_H */
