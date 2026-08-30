/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/save_participant.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Save Participant document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/save_participant.h"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_save_participant(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_SAVE_PARTICIPANT,
        .title = "Save Participant",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_PERSISTENCE,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_PLANNED,
        .summary = "Defines safe persistence policy including atomic replacement and user-visible save behaviour.",
        .provider_role = "framework-extension",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_SECURITY_SENSITIVE | UMI_DOCUMENT_CAPABILITY_FLAG_PERSISTED,
        .priority = 350
    };
    return &descriptor;
}
