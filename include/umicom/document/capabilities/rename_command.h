/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/rename_command.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Rename Command document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_RENAME_COMMAND_H
#define UMICOM_DOCUMENT_CAPABILITIES_RENAME_COMMAND_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_RENAME_COMMAND "umicom.document.command.rename_command"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_rename_command(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_RENAME_COMMAND_H */
