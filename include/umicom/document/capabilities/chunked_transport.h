/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/chunked_transport.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Chunked Transport document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_CHUNKED_TRANSPORT_H
#define UMICOM_DOCUMENT_CAPABILITIES_CHUNKED_TRANSPORT_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_CHUNKED_TRANSPORT "umicom.document.performance.chunked_transport"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_chunked_transport(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_CHUNKED_TRANSPORT_H */
