/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/symlink_policy.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Symlink Policy document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_SYMLINK_POLICY_H
#define UMICOM_DOCUMENT_CAPABILITIES_SYMLINK_POLICY_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_SYMLINK_POLICY "umicom.document.security.symlink_policy"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_symlink_policy(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_SYMLINK_POLICY_H */
