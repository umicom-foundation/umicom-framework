/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/session_restore.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Session Restore document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/session_restore.h"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_session_restore(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_SESSION_RESTORE,
        .title = "Session Restore",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_RECOVERY,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_PLANNED,
        .summary = "Defines recoverable working-copy state and restart behaviour after normal or abnormal termination.",
        .provider_role = "framework-extension",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_SECURITY_SENSITIVE | UMI_DOCUMENT_CAPABILITY_FLAG_PERSISTED,
        .priority = 470
    };
    return &descriptor;
}
