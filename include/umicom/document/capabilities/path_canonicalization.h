/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/path_canonicalization.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Path Canonicalization document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_PATH_CANONICALIZATION_H
#define UMICOM_DOCUMENT_CAPABILITIES_PATH_CANONICALIZATION_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_PATH_CANONICALIZATION "umicom.document.identity.path_canonicalization"

/**
 * Provide the document capability path canonicalization operation used by this module and
 * its client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_path_canonicalization(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_PATH_CANONICALIZATION_H */
