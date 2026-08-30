/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/patch_builder.c
 *
 * PURPOSE:
 *   Implement bounded unified-patch composition for exports and partial staging.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/advanced/patch_builder.h"

#include <stdio.h>
#include <string.h>

void umi_vcs_advanced_patch_builder_init(UmiVcsAdvancedPatchBuilder *builder)
{
    if (builder == NULL) {
        return;
    }
    (void)memset(builder, 0, sizeof(*builder));
    builder->struct_size = (uint32_t)sizeof(*builder);
    builder->api_version = UMI_VCS_ADVANCED_API_VERSION;
}

UmiStatus umi_vcs_advanced_patch_builder_append(
    UmiVcsAdvancedPatchBuilder *builder,
    const char *text)
{
    size_t length;
    if (builder == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(text);
    if (builder->length + length >= sizeof(builder->text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(builder->text + builder->length, text, length + 1U);
    builder->length += length;
    builder->fingerprint = umi_vcs_advanced_hash_text(builder->text);
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_advanced_patch_builder_begin_file(
    UmiVcsAdvancedPatchBuilder *builder,
    const char *left,
    const char *right)
{
    char header[2300];
    int written;
    UmiStatus status;

    if (builder == NULL ||
        !umi_vcs_advanced_text_present(left) ||
        !umi_vcs_advanced_text_present(right)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    written = snprintf(
        header,
        sizeof(header),
        "diff --git a/%s b/%s\n--- a/%s\n+++ b/%s\n",
        left,
        right,
        left,
        right);
    if (written < 0 || (size_t)written >= sizeof(header)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = umi_vcs_advanced_patch_builder_append(builder, header);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    builder->file_count += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_advanced_patch_builder_add_hunk(
    UmiVcsAdvancedPatchBuilder *builder,
    const char *hunk_text)
{
    UmiStatus status;
    if (builder == NULL || !umi_vcs_advanced_text_present(hunk_text)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_vcs_advanced_patch_builder_append(builder, hunk_text);
    if (status == UMI_STATUS_OK) {
        builder->hunk_count += 1U;
    }
    return status;
}

const char *umi_vcs_advanced_patch_builder_text(
    const UmiVcsAdvancedPatchBuilder *builder)
{
    return builder != NULL ? builder->text : "";
}
