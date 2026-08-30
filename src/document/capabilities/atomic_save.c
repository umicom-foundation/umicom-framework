/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/atomic_save.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Atomic Save document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/atomic_save.h"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_atomic_save(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_ATOMIC_SAVE,
        .title = "Atomic Save",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_PERSISTENCE,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE,
        .summary = "Defines safe persistence policy including atomic replacement and user-visible save behaviour.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_SECURITY_SENSITIVE | UMI_DOCUMENT_CAPABILITY_FLAG_PERSISTED | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 330
    };
    return &descriptor;
}
