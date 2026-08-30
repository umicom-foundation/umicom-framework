/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/final_newline_policy.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Final Newline Policy document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_FINAL_NEWLINE_POLICY_H
#define UMICOM_DOCUMENT_CAPABILITIES_FINAL_NEWLINE_POLICY_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_FINAL_NEWLINE_POLICY "umicom.document.persistence.final_newline_policy"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_final_newline_policy(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_FINAL_NEWLINE_POLICY_H */
