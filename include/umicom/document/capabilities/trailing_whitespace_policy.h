/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/trailing_whitespace_policy.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Trailing Whitespace Policy document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_TRAILING_WHITESPACE_POLICY_H
#define UMICOM_DOCUMENT_CAPABILITIES_TRAILING_WHITESPACE_POLICY_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_TRAILING_WHITESPACE_POLICY "umicom.document.persistence.trailing_whitespace_policy"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_trailing_whitespace_policy(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_TRAILING_WHITESPACE_POLICY_H */
