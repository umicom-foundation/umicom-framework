/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/binary_file_policy.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Binary File Policy document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_BINARY_FILE_POLICY_H
#define UMICOM_DOCUMENT_CAPABILITIES_BINARY_FILE_POLICY_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_BINARY_FILE_POLICY "umicom.document.security.binary_file_policy"

/**
 * Provide the document capability binary file policy operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_binary_file_policy(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_BINARY_FILE_POLICY_H */
