/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/local_file_provider.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Local File Provider document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_LOCAL_FILE_PROVIDER_H
#define UMICOM_DOCUMENT_CAPABILITIES_LOCAL_FILE_PROVIDER_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_LOCAL_FILE_PROVIDER "umicom.document.provider.local_file_provider"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_local_file_provider(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_LOCAL_FILE_PROVIDER_H */
