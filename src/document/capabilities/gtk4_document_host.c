/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/gtk4_document_host.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Gtk4 Document Host document capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/gtk4_document_host.h"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_gtk4_document_host(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_GTK4_DOCUMENT_HOST,
        .title = "Gtk4 Document Host",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_INTEGRATION,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE,
        .summary = "Defines an adapter boundary connecting the document core to UI, language, VCS, tests and telemetry.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 910
    };
    return &descriptor;
}
