/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/incremental_fingerprint.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Incremental Fingerprint document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_INCREMENTAL_FINGERPRINT_H
#define UMICOM_DOCUMENT_CAPABILITIES_INCREMENTAL_FINGERPRINT_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_INCREMENTAL_FINGERPRINT "umicom.document.performance.incremental_fingerprint"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_incremental_fingerprint(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_INCREMENTAL_FINGERPRINT_H */
