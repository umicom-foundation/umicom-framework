/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/new_command.h
 *
 * PURPOSE:
 *   Declare the Framework-owned New Command document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_NEW_COMMAND_H
#define UMICOM_DOCUMENT_CAPABILITIES_NEW_COMMAND_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_NEW_COMMAND "umicom.document.command.new_command"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_new_command(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_NEW_COMMAND_H */
