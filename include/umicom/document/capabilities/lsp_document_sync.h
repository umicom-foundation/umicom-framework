/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/lsp_document_sync.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Lsp Document Sync document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_LSP_DOCUMENT_SYNC_H
#define UMICOM_DOCUMENT_CAPABILITIES_LSP_DOCUMENT_SYNC_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_LSP_DOCUMENT_SYNC "umicom.document.integration.lsp_document_sync"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_lsp_document_sync(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_LSP_DOCUMENT_SYNC_H */
