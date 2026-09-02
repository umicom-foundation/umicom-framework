/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/bom_policy.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Bom Policy document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_BOM_POLICY_H
#define UMICOM_DOCUMENT_CAPABILITIES_BOM_POLICY_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_BOM_POLICY "umicom.document.encoding.bom_policy"

/**
 * Provide the document capability bom policy operation used by this module and its client
 * applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_bom_policy(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_BOM_POLICY_H */
