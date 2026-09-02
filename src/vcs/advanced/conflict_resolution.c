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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/advanced/conflict_resolution.h"

#include <string.h>

/*
 * Initialise vcs advanced conflict resolution from caller-provided values so later
 * operations receive a known state.
 */
void umi_vcs_advanced_conflict_resolution_init(
    UmiVcsAdvancedConflictResolution *resolution)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (resolution == NULL) {
        return;
    }
    (void)memset(resolution, 0, sizeof(*resolution));
    resolution->struct_size = (uint32_t)sizeof(*resolution);
    resolution->api_version = UMI_VCS_ADVANCED_API_VERSION;
    resolution->state = UMI_VCS_ADVANCED_STATE_CONFLICTED;
}

/*
 * Provide the vcs advanced conflict resolution set path operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_advanced_conflict_resolution_set_path(
    UmiVcsAdvancedConflictResolution *resolution,
    const char *path)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (resolution == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_vcs_advanced_copy_text(
        resolution->path, sizeof(resolution->path), path);
}

/*
 * Add vcs advanced conflict resolution only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_vcs_advanced_conflict_resolution_add(
    UmiVcsAdvancedConflictResolution *resolution,
    const UmiVcsAdvancedConflictHunk *hunk)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (resolution == NULL ||
        umi_vcs_advanced_conflict_hunk_validate(hunk) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (resolution->hunk_count >= UMI_VCS_ADVANCED_SMALL_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    resolution->hunks[resolution->hunk_count++] = *hunk;
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs advanced conflict resolution choose operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_advanced_conflict_resolution_choose(
    UmiVcsAdvancedConflictResolution *resolution,
    size_t index,
    UmiVcsConflictChoice choice)
{
    UmiVcsConflictChoice previous;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (resolution == NULL ||
        index >= resolution->hunk_count ||
        choice == UMI_VCS_CONFLICT_UNRESOLVED ||
        choice > UMI_VCS_CONFLICT_MANUAL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    previous = resolution->hunks[index].choice;
    resolution->hunks[index].choice = choice;
    /* Apply this branch only when its contract condition is satisfied. */
    if (previous == UMI_VCS_CONFLICT_UNRESOLVED) {
        resolution->resolved_count += 1U;
    }
    resolution->result_fingerprint ^=
        resolution->hunks[index].fingerprint ^ ((uint64_t)choice << 8);

    /* Apply this branch only when its contract condition is satisfied. */
    if (resolution->resolved_count == resolution->hunk_count) {
        resolution->state = UMI_VCS_ADVANCED_STATE_READY;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs advanced conflict resolution complete operation used by this module and
 * its client applications.
 */
int umi_vcs_advanced_conflict_resolution_complete(
    const UmiVcsAdvancedConflictResolution *resolution)
{
    return resolution != NULL &&
           resolution->hunk_count > 0U &&
           resolution->resolved_count == resolution->hunk_count;
}
