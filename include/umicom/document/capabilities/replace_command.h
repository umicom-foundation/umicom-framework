/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/replace_command.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Replace Command document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_REPLACE_COMMAND_H
#define UMICOM_DOCUMENT_CAPABILITIES_REPLACE_COMMAND_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_REPLACE_COMMAND "umicom.document.navigation.replace_command"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_replace_command(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_REPLACE_COMMAND_H */
