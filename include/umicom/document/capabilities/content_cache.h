/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/content_cache.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Content Cache document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_CONTENT_CACHE_H
#define UMICOM_DOCUMENT_CAPABILITIES_CONTENT_CACHE_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_CONTENT_CACHE "umicom.document.performance.content_cache"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_content_cache(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_CONTENT_CACHE_H */
