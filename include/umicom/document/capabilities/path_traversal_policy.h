/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/path_traversal_policy.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Path Traversal Policy document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_PATH_TRAVERSAL_POLICY_H
#define UMICOM_DOCUMENT_CAPABILITIES_PATH_TRAVERSAL_POLICY_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_PATH_TRAVERSAL_POLICY "umicom.document.security.path_traversal_policy"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_path_traversal_policy(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_PATH_TRAVERSAL_POLICY_H */
