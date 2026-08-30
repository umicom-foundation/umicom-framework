/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/clone_document.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Clone Document document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_CLONE_DOCUMENT_H
#define UMICOM_DOCUMENT_CAPABILITIES_CLONE_DOCUMENT_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_CLONE_DOCUMENT "umicom.document.lifecycle.clone_document"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_clone_document(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_CLONE_DOCUMENT_H */
