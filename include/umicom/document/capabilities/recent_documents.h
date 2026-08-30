/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/recent_documents.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Recent Documents document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_RECENT_DOCUMENTS_H
#define UMICOM_DOCUMENT_CAPABILITIES_RECENT_DOCUMENTS_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_RECENT_DOCUMENTS "umicom.document.navigation.recent_documents"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_recent_documents(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_RECENT_DOCUMENTS_H */
