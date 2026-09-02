/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/memory_provider.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Memory Provider document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_MEMORY_PROVIDER_H
#define UMICOM_DOCUMENT_CAPABILITIES_MEMORY_PROVIDER_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_MEMORY_PROVIDER "umicom.document.provider.memory_provider"

/**
 * Provide the document capability memory provider operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_memory_provider(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_MEMORY_PROVIDER_H */
