/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/protocol/lsp_document.h
 *
 * PURPOSE:
 *   Track LSP document URIs, language identifiers and versions independently from editor widgets.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROTOCOL_LSP_DOCUMENT_H
#define UMICOM_PROTOCOL_LSP_DOCUMENT_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/protocol/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiLspDocument {
    char uri[UMI_PROTOCOL_URI_CAPACITY];
    char language_id[UMI_PROTOCOL_LANGUAGE_CAPACITY];
    int version;
    int open;
} UmiLspDocument;

typedef struct UmiLspDocumentRegistry UmiLspDocumentRegistry;

UmiStatus umi_lsp_document_registry_create(
    UmiLspDocumentRegistry **out_registry
);
void umi_lsp_document_registry_destroy(UmiLspDocumentRegistry *registry);
UmiStatus umi_lsp_document_registry_open(
    UmiLspDocumentRegistry *registry,
    const char *uri,
    const char *language_id,
    int version
);
UmiStatus umi_lsp_document_registry_change(
    UmiLspDocumentRegistry *registry,
    const char *uri,
    int version
);
UmiStatus umi_lsp_document_registry_close(
    UmiLspDocumentRegistry *registry,
    const char *uri
);
const UmiLspDocument *umi_lsp_document_registry_find(
    const UmiLspDocumentRegistry *registry,
    const char *uri
);
size_t umi_lsp_document_registry_count(
    const UmiLspDocumentRegistry *registry
);

#ifdef __cplusplus
}
#endif

#endif
