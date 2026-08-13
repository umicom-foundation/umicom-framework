/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/find_command.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Find Command document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_FIND_COMMAND_H
#define UMICOM_DOCUMENT_CAPABILITIES_FIND_COMMAND_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_FIND_COMMAND "umicom.document.navigation.find_command"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_find_command(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_FIND_COMMAND_H */
