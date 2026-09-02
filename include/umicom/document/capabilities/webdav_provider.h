/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/webdav_provider.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Webdav Provider document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_WEBDAV_PROVIDER_H
#define UMICOM_DOCUMENT_CAPABILITIES_WEBDAV_PROVIDER_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_WEBDAV_PROVIDER "umicom.document.provider.webdav_provider"

/**
 * Provide the document capability webdav provider operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_webdav_provider(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_WEBDAV_PROVIDER_H */
