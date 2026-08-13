/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/content_fingerprint.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Content Fingerprint document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_CONTENT_FINGERPRINT_H
#define UMICOM_DOCUMENT_CAPABILITIES_CONTENT_FINGERPRINT_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_CONTENT_FINGERPRINT "umicom.document.identity.content_fingerprint"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_content_fingerprint(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_CONTENT_FINGERPRINT_H */
