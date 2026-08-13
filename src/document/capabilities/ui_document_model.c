/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/ui_document_model.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Ui Document Model document capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/ui_document_model.h"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_ui_document_model(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_UI_DOCUMENT_MODEL,
        .title = "Ui Document Model",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_INTEGRATION,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE,
        .summary = "Defines an adapter boundary connecting the document core to UI, language, VCS, tests and telemetry.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 890
    };
    return &descriptor;
}
