/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/lsp_document_sync.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Lsp Document Sync document capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/lsp_document_sync.h"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_lsp_document_sync(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_LSP_DOCUMENT_SYNC,
        .title = "Lsp Document Sync",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_INTEGRATION,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_PLANNED,
        .summary = "Defines an adapter boundary connecting the document core to UI, language, VCS, tests and telemetry.",
        .provider_role = "framework-extension",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI,
        .priority = 920
    };
    return &descriptor;
}
