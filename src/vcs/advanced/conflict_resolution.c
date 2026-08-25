/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/conflict_resolution.c
 *
 * PURPOSE:
 *   Implement per-hunk conflict choices and completion state.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/advanced/conflict_resolution.h"

#include <string.h>

void umi_vcs_advanced_conflict_resolution_init(
    UmiVcsAdvancedConflictResolution *resolution)
{
    if (resolution == NULL) {
        return;
    }
    (void)memset(resolution, 0, sizeof(*resolution));
    resolution->struct_size = (uint32_t)sizeof(*resolution);
    resolution->api_version = UMI_VCS_ADVANCED_API_VERSION;
    resolution->state = UMI_VCS_ADVANCED_STATE_CONFLICTED;
}

UmiStatus umi_vcs_advanced_conflict_resolution_set_path(
    UmiVcsAdvancedConflictResolution *resolution,
    const char *path)
{
    if (resolution == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_vcs_advanced_copy_text(
        resolution->path, sizeof(resolution->path), path);
}

UmiStatus umi_vcs_advanced_conflict_resolution_add(
    UmiVcsAdvancedConflictResolution *resolution,
    const UmiVcsAdvancedConflictHunk *hunk)
{
    if (resolution == NULL ||
        umi_vcs_advanced_conflict_hunk_validate(hunk) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (resolution->hunk_count >= UMI_VCS_ADVANCED_SMALL_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    resolution->hunks[resolution->hunk_count++] = *hunk;
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_advanced_conflict_resolution_choose(
    UmiVcsAdvancedConflictResolution *resolution,
    size_t index,
    UmiVcsConflictChoice choice)
{
    UmiVcsConflictChoice previous;
    if (resolution == NULL ||
        index >= resolution->hunk_count ||
        choice == UMI_VCS_CONFLICT_UNRESOLVED ||
        choice > UMI_VCS_CONFLICT_MANUAL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    previous = resolution->hunks[index].choice;
    resolution->hunks[index].choice = choice;
    if (previous == UMI_VCS_CONFLICT_UNRESOLVED) {
        resolution->resolved_count += 1U;
    }
    resolution->result_fingerprint ^=
        resolution->hunks[index].fingerprint ^ ((uint64_t)choice << 8);

    if (resolution->resolved_count == resolution->hunk_count) {
        resolution->state = UMI_VCS_ADVANCED_STATE_READY;
    }
    return UMI_STATUS_OK;
}

int umi_vcs_advanced_conflict_resolution_complete(
    const UmiVcsAdvancedConflictResolution *resolution)
{
    return resolution != NULL &&
           resolution->hunk_count > 0U &&
           resolution->resolved_count == resolution->hunk_count;
}
