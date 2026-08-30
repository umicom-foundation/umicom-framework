/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/archive_provider.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Archive Provider document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_ARCHIVE_PROVIDER_H
#define UMICOM_DOCUMENT_CAPABILITIES_ARCHIVE_PROVIDER_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_ARCHIVE_PROVIDER "umicom.document.provider.archive_provider"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_archive_provider(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_ARCHIVE_PROVIDER_H */
