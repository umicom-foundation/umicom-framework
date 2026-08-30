/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/headless_document_host.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Headless Document Host document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_HEADLESS_DOCUMENT_HOST_H
#define UMICOM_DOCUMENT_CAPABILITIES_HEADLESS_DOCUMENT_HOST_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_HEADLESS_DOCUMENT_HOST "umicom.document.integration.headless_document_host"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_headless_document_host(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_HEADLESS_DOCUMENT_HOST_H */
