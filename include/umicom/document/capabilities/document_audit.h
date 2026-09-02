/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/document_audit.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Document Audit document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_DOCUMENT_AUDIT_H
#define UMICOM_DOCUMENT_CAPABILITIES_DOCUMENT_AUDIT_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_DOCUMENT_AUDIT "umicom.document.security.document_audit"

/**
 * Provide the document capability document audit operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_document_audit(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_DOCUMENT_AUDIT_H */
