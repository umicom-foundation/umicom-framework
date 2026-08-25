/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/repository_snapshot.c
 *
 * PURPOSE:
 *   Implement compose all read-only probe outputs into the existing maintenance summary plus inspection metadata.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/repository/repository_snapshot.h"

#include "umicom/repository/attributes_probe.h"
#include "umicom/repository/fetch_state_probe.h"
#include "umicom/repository/gitlink_probe.h"
#include "umicom/repository/gitmodules_probe.h"
#include "umicom/repository/head_probe.h"
#include "umicom/repository/ignore_probe.h"
#include "umicom/repository/nested_repository_probe.h"
#include "umicom/repository/remote_probe.h"
#include "umicom/repository/repository_identity.h"
#include "umicom/repository/status_probe.h"
#include "umicom/repository/submodule_probe.h"
#include "umicom/repository/tag_probe.h"

/*
 * Collect independent repository evidence and project it into the existing
 * maintenance summary. Optional metadata probes are deliberately non-fatal.
 */
UmiStatus umi_repository_snapshot_collect(
    const UmiRepositoryInspectionContext *context,
    UmiRepositoryInspectionSnapshot *out_snapshot)
{
    UmiRepositoryInventory inventory;
    UmiRepositoryPorcelainStatus porcelain;
    UmiStatus status;

    /* A complete snapshot requires both an immutable context and output model. */
    if (context == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_repository_inspection_snapshot_init(out_snapshot);
    umi_repository_inventory_init(&inventory);

    /* Read branch, worktree and index from one identical porcelain-v2 snapshot. */
    status = umi_repository_status_probe_read(context, &porcelain);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    out_snapshot->maintenance.branch = porcelain.branch;
    out_snapshot->maintenance.worktree = porcelain.worktree;
    out_snapshot->maintenance.index = porcelain.index;

    /* Read remote and submodule health through their established state models. */
    status = umi_repository_remote_probe_read(
        context, &out_snapshot->maintenance.remotes);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    out_snapshot->maintenance.remotes.upstream_configured =
        out_snapshot->maintenance.branch.upstream[0] != '\0';

    status = umi_repository_submodule_probe_read(
        context, &out_snapshot->maintenance.submodules);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    if (porcelain.submodule_dirty > out_snapshot->maintenance.submodules.dirty) {
        out_snapshot->maintenance.submodules.dirty = porcelain.submodule_dirty;
    }

    /* Optional identity and policy-file evidence enriches, but cannot erase, core status. */
    (void)umi_repository_repository_identity_read(
        context, out_snapshot->identity.root, sizeof(out_snapshot->identity.root));
    (void)umi_repository_head_probe_read(
        context, out_snapshot->identity.head, sizeof(out_snapshot->identity.head));
    if (umi_repository_tag_probe_read(
            context, out_snapshot->identity.exact_tag,
            sizeof(out_snapshot->identity.exact_tag)) == UMI_STATUS_OK &&
        out_snapshot->identity.exact_tag[0] != '\0') {
        out_snapshot->tag_count_at_head = 1U;
    }
    (void)umi_repository_fetch_state_probe_read(
        context, &out_snapshot->upstream_reachable_locally);
    (void)umi_repository_gitmodules_probe_read(
        context, &inventory, &out_snapshot->has_gitmodules);
    out_snapshot->configured_submodules = inventory.count;
    (void)umi_repository_gitlink_probe_read(
        context, &out_snapshot->tracked_gitlinks);
    (void)umi_repository_ignore_probe_read(
        context, &out_snapshot->has_gitignore);
    (void)umi_repository_attributes_probe_read(
        context, &out_snapshot->has_gitattributes);
    (void)umi_repository_nested_repository_probe_read(
        context, &out_snapshot->has_superproject);

    /* Reuse the established Framework maintenance projection exactly once. */
    status = umi_repository_status_summary_refresh(&out_snapshot->maintenance);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    out_snapshot->revision = 1U;
    return UMI_STATUS_OK;
}
