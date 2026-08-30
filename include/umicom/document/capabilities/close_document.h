/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/close_document.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Close Document document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_CLOSE_DOCUMENT_H
#define UMICOM_DOCUMENT_CAPABILITIES_CLOSE_DOCUMENT_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_CLOSE_DOCUMENT "umicom.document.lifecycle.close_document"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_close_document(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_CLOSE_DOCUMENT_H */
