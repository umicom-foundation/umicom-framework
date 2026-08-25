/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/three_way_merge.c
 *
 * PURPOSE:
 *   Implement conservative whole-text three-way merge with explicit conflict markers.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/advanced/three_way_merge.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_vcs_advanced_three_way_merge_text(
    const char *base,
    const char *ours,
    const char *theirs,
    UmiVcsAdvancedThreeWayMergeResult *out_result)
{
    int written;
    UmiStatus status;

    if (base == NULL || ours == NULL || theirs == NULL ||
        out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_result, 0, sizeof(*out_result));
    out_result->struct_size = (uint32_t)sizeof(*out_result);
    out_result->api_version = UMI_VCS_ADVANCED_API_VERSION;

    if (strcmp(ours, theirs) == 0) {
        status = umi_vcs_advanced_copy_text(
            out_result->text, sizeof(out_result->text), ours);
        if (status != UMI_STATUS_OK) {
            return status;
        }
        out_result->clean = 1;
    } else if (strcmp(base, ours) == 0) {
        status = umi_vcs_advanced_copy_text(
            out_result->text, sizeof(out_result->text), theirs);
        if (status != UMI_STATUS_OK) {
            return status;
        }
        out_result->clean = 1;
    } else if (strcmp(base, theirs) == 0) {
        status = umi_vcs_advanced_copy_text(
            out_result->text, sizeof(out_result->text), ours);
        if (status != UMI_STATUS_OK) {
            return status;
        }
        out_result->clean = 1;
    } else {
        written = snprintf(
            out_result->text,
            sizeof(out_result->text),
            "<<<<<<< ours\n%s\n||||||| base\n%s\n=======\n%s\n>>>>>>> theirs\n",
            ours,
            base,
            theirs);
        if (written < 0 || (size_t)written >= sizeof(out_result->text)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        out_result->conflict_count = 1U;
        out_result->clean = 0;
    }

    out_result->fingerprint = umi_vcs_advanced_hash_text(out_result->text);
    return UMI_STATUS_OK;
}
