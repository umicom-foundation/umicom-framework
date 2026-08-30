/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/open_document.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Open Document document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_OPEN_DOCUMENT_H
#define UMICOM_DOCUMENT_CAPABILITIES_OPEN_DOCUMENT_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_OPEN_DOCUMENT "umicom.document.lifecycle.open_document"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_open_document(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_OPEN_DOCUMENT_H */
