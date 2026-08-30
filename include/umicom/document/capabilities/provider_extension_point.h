/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/provider_extension_point.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Provider Extension Point document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_PROVIDER_EXTENSION_POINT_H
#define UMICOM_DOCUMENT_CAPABILITIES_PROVIDER_EXTENSION_POINT_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_PROVIDER_EXTENSION_POINT "umicom.document.integration.provider_extension_point"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_provider_extension_point(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_PROVIDER_EXTENSION_POINT_H */
