/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/asynchronous_save.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Asynchronous Save document capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/asynchronous_save.h"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_asynchronous_save(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_ASYNCHRONOUS_SAVE,
        .title = "Asynchronous Save",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_PERFORMANCE,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_PLANNED,
        .summary = "Defines scalable and cancellable document I/O behaviour for large or remote resources.",
        .provider_role = "framework-extension",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI,
        .priority = 850
    };
    return &descriptor;
}
