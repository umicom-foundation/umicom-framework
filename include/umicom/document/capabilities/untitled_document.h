/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/untitled_document.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Untitled Document document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_UNTITLED_DOCUMENT_H
#define UMICOM_DOCUMENT_CAPABILITIES_UNTITLED_DOCUMENT_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_UNTITLED_DOCUMENT "umicom.document.lifecycle.untitled_document"

/**
 * Provide the document capability untitled document operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_untitled_document(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_UNTITLED_DOCUMENT_H */
