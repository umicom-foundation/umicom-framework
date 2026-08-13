/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/binary_file_policy.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Binary File Policy document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_BINARY_FILE_POLICY_H
#define UMICOM_DOCUMENT_CAPABILITIES_BINARY_FILE_POLICY_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_BINARY_FILE_POLICY "umicom.document.security.binary_file_policy"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_binary_file_policy(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_BINARY_FILE_POLICY_H */
