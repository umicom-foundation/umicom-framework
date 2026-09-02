/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/document_history.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Document History document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_DOCUMENT_HISTORY_H
#define UMICOM_DOCUMENT_CAPABILITIES_DOCUMENT_HISTORY_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_DOCUMENT_HISTORY "umicom.document.navigation.document_history"

/**
 * Provide the document capability document history operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_document_history(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_DOCUMENT_HISTORY_H */
