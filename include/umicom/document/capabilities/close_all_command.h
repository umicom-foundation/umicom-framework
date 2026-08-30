/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/close_all_command.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Close All Command document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_CLOSE_ALL_COMMAND_H
#define UMICOM_DOCUMENT_CAPABILITIES_CLOSE_ALL_COMMAND_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_CLOSE_ALL_COMMAND "umicom.document.command.close_all_command"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_close_all_command(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_CLOSE_ALL_COMMAND_H */
