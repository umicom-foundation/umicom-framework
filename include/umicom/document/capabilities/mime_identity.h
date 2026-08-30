/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/mime_identity.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Mime Identity document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_MIME_IDENTITY_H
#define UMICOM_DOCUMENT_CAPABILITIES_MIME_IDENTITY_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_MIME_IDENTITY "umicom.document.identity.mime_identity"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_mime_identity(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_MIME_IDENTITY_H */
