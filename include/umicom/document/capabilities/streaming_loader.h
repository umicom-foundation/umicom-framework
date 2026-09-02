/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/streaming_loader.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Streaming Loader document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_STREAMING_LOADER_H
#define UMICOM_DOCUMENT_CAPABILITIES_STREAMING_LOADER_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_STREAMING_LOADER "umicom.document.performance.streaming_loader"

/**
 * Provide the document capability streaming loader operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_streaming_loader(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_STREAMING_LOADER_H */
