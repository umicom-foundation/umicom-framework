/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/virtual_document.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Virtual Document document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_VIRTUAL_DOCUMENT_H
#define UMICOM_DOCUMENT_CAPABILITIES_VIRTUAL_DOCUMENT_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_VIRTUAL_DOCUMENT "umicom.document.lifecycle.virtual_document"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_virtual_document(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_VIRTUAL_DOCUMENT_H */
