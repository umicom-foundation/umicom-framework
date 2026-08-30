/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/resource_uri.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Resource Uri document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_RESOURCE_URI_H
#define UMICOM_DOCUMENT_CAPABILITIES_RESOURCE_URI_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_RESOURCE_URI "umicom.document.identity.resource_uri"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_resource_uri(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_RESOURCE_URI_H */
