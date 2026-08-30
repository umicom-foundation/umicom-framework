/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/reopen_document.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Reopen Document document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_REOPEN_DOCUMENT_H
#define UMICOM_DOCUMENT_CAPABILITIES_REOPEN_DOCUMENT_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_REOPEN_DOCUMENT "umicom.document.lifecycle.reopen_document"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_reopen_document(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_REOPEN_DOCUMENT_H */
