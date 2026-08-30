/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/layout_snapshot.c
 *
 * PURPOSE:
 *   Implement immutable layout snapshot metadata used by persistence, autosave, recovery and visual history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/layout_snapshot.h"

UmiStatus umi_ws_layout_snapshot_capture(UmiWsLayoutSnapshot *snapshot,
                                         const char *snapshot_id,
                                         const UmiWsLayoutTemplate *layout,
                                         uint64_t sequence,
                                         int64_t timestamp_ms,
                                         bool autosave) {
    if (snapshot == NULL || !umi_ws_id_valid(snapshot_id) || layout == NULL || !umi_ws_id_valid(layout->layout_id)) return UMI_STATUS_INVALID_ARGUMENT;
    *snapshot = (UmiWsLayoutSnapshot){0};
    if (umi_ws_copy_text(snapshot->snapshot_id, sizeof(snapshot->snapshot_id), snapshot_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_ws_copy_text(snapshot->layout_id, sizeof(snapshot->layout_id), layout->layout_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    snapshot->layout_fingerprint = umi_ws_layout_template_fingerprint(layout);
    snapshot->sequence = sequence;
    snapshot->timestamp_ms = timestamp_ms;
    snapshot->autosave = autosave;
    return UMI_STATUS_OK;
}

bool umi_ws_layout_snapshot_matches(const UmiWsLayoutSnapshot *snapshot, const UmiWsLayoutTemplate *layout) {
    if (snapshot == NULL || layout == NULL) return false;
    return snapshot->layout_fingerprint == umi_ws_layout_template_fingerprint(layout);
}
