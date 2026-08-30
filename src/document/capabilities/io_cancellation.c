/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/io_cancellation.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Io Cancellation document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/io_cancellation.h"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_io_cancellation(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_IO_CANCELLATION,
        .title = "Io Cancellation",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_PERFORMANCE,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE,
        .summary = "Defines scalable and cancellable document I/O behaviour for large or remote resources.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 880
    };
    return &descriptor;
}
