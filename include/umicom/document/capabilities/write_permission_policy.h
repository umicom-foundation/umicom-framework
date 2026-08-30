/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/write_permission_policy.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Write Permission Policy document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_WRITE_PERMISSION_POLICY_H
#define UMICOM_DOCUMENT_CAPABILITIES_WRITE_PERMISSION_POLICY_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_WRITE_PERMISSION_POLICY "umicom.document.security.write_permission_policy"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_write_permission_policy(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_WRITE_PERMISSION_POLICY_H */
