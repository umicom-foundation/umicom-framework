/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/sensitive_content_redaction.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Sensitive Content Redaction document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_SENSITIVE_CONTENT_REDACTION_H
#define UMICOM_DOCUMENT_CAPABILITIES_SENSITIVE_CONTENT_REDACTION_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_SENSITIVE_CONTENT_REDACTION "umicom.document.security.sensitive_content_redaction"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_sensitive_content_redaction(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_SENSITIVE_CONTENT_REDACTION_H */
