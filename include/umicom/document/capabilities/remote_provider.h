/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/remote_provider.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Remote Provider document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_REMOTE_PROVIDER_H
#define UMICOM_DOCUMENT_CAPABILITIES_REMOTE_PROVIDER_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_REMOTE_PROVIDER "umicom.document.provider.remote_provider"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_remote_provider(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_REMOTE_PROVIDER_H */
