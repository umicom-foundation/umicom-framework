/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/incremental_fingerprint.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Incremental Fingerprint document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_INCREMENTAL_FINGERPRINT_H
#define UMICOM_DOCUMENT_CAPABILITIES_INCREMENTAL_FINGERPRINT_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_INCREMENTAL_FINGERPRINT "umicom.document.performance.incremental_fingerprint"

/**
 * Provide the document capability incremental fingerprint operation used by this module
 * and its client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_incremental_fingerprint(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_INCREMENTAL_FINGERPRINT_H */
