/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/backup_copy.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Backup Copy document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/backup_copy.h"

/*
 * Copy document capability backup into module-owned storage so callers keep ownership of
 * their input values.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_backup_copy(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_BACKUP_COPY,
        .title = "Backup Copy",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_PERSISTENCE,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_PLANNED,
        .summary = "Defines safe persistence policy including atomic replacement and user-visible save behaviour.",
        .provider_role = "framework-extension",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_SECURITY_SENSITIVE | UMI_DOCUMENT_CAPABILITY_FLAG_PERSISTED,
        .priority = 360
    };
    return &descriptor;
}
