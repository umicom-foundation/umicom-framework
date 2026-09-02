/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/protocol/lsp_document.h
 *
 * PURPOSE:
 *   Track LSP document URIs, language identifiers and versions independently from editor widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROTOCOL_LSP_DOCUMENT_H
#define UMICOM_PROTOCOL_LSP_DOCUMENT_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/protocol/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the lsp document data shared with callers of this public contract.
 */
typedef struct UmiLspDocument {
    char uri[UMI_PROTOCOL_URI_CAPACITY];
    char language_id[UMI_PROTOCOL_LANGUAGE_CAPACITY];
    int version;
    int open;
} UmiLspDocument;

/**
 * Represent the lsp document registry data shared with callers of this public contract.
 */
typedef struct UmiLspDocumentRegistry UmiLspDocumentRegistry;

/**
 * Initialise lsp document registry from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_lsp_document_registry_create(
    UmiLspDocumentRegistry **out_registry
);
/**
 * Release or reset state held by lsp document registry so the same storage can be reused
 * safely.
 */
void umi_lsp_document_registry_destroy(UmiLspDocumentRegistry *registry);
/**
 * Provide the lsp document registry open operation used by this module and its client
 * applications.
 */
UmiStatus umi_lsp_document_registry_open(
    UmiLspDocumentRegistry *registry,
    const char *uri,
    const char *language_id,
    int version
);
/**
 * Provide the lsp document registry change operation used by this module and its client
 * applications.
 */
UmiStatus umi_lsp_document_registry_change(
    UmiLspDocumentRegistry *registry,
    const char *uri,
    int version
);
/**
 * Provide the lsp document registry close operation used by this module and its client
 * applications.
 */
UmiStatus umi_lsp_document_registry_close(
    UmiLspDocumentRegistry *registry,
    const char *uri
);
/**
 * Find lsp document registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiLspDocument *umi_lsp_document_registry_find(
    const UmiLspDocumentRegistry *registry,
    const char *uri
);
/**
 * Return the number of records represented by lsp document registry without changing their
 * state.
 */
size_t umi_lsp_document_registry_count(
    const UmiLspDocumentRegistry *registry
);

#ifdef __cplusplus
}
#endif

#endif
