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

/*
 * Initialise vcs advanced patch builder from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_patch_builder_init(UmiVcsAdvancedPatchBuilder *builder)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (builder == NULL) {
        return;
    }
    (void)memset(builder, 0, sizeof(*builder));
    builder->struct_size = (uint32_t)sizeof(*builder);
    builder->api_version = UMI_VCS_ADVANCED_API_VERSION;
}

/*
 * Add vcs advanced patch builder only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_vcs_advanced_patch_builder_append(
    UmiVcsAdvancedPatchBuilder *builder,
    const char *text)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (builder == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (builder->length + length >= sizeof(builder->text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(builder->text + builder->length, text, length + 1U);
    builder->length += length;
    builder->fingerprint = umi_vcs_advanced_hash_text(builder->text);
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs advanced patch builder begin file operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_advanced_patch_builder_begin_file(
    UmiVcsAdvancedPatchBuilder *builder,
    const char *left,
    const char *right)
{
    char header[2300];
    int written;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(header)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = umi_vcs_advanced_patch_builder_append(builder, header);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    builder->file_count += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs advanced patch builder add hunk operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_advanced_patch_builder_add_hunk(
    UmiVcsAdvancedPatchBuilder *builder,
    const char *hunk_text)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (builder == NULL || !umi_vcs_advanced_text_present(hunk_text)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_vcs_advanced_patch_builder_append(builder, hunk_text);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        builder->hunk_count += 1U;
    }
    return status;
}

/*
 * Provide the vcs advanced patch builder text operation used by this module and its client
 * applications.
 */
const char *umi_vcs_advanced_patch_builder_text(
    const UmiVcsAdvancedPatchBuilder *builder)
{
    return builder != NULL ? builder->text : "";
}
