/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/display_name.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Display Name document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_DISPLAY_NAME_H
#define UMICOM_DOCUMENT_CAPABILITIES_DISPLAY_NAME_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_DISPLAY_NAME "umicom.document.identity.display_name"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_display_name(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_DISPLAY_NAME_H */
