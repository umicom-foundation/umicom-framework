/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_closing/test_editor_history_transaction.c
 * PURPOSE: Verify host-first navigation, stale revisions and callback re-entry.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/navigation_history.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); failed = 1; goto cleanup; } } while (0)

typedef struct Host {
    UmiEditorNavigationHistory *history;
    UmiEditorSourceLocation target;
    UmiStatus result;
    unsigned calls;
    int nested, guarded;
} Host;

/* Exercise all mutating entry points from the host boundary. Reading remains
 * available, but no nested command may change the transaction's destination. */
static UmiStatus Apply(void *context, const UmiEditorSourceLocation *target)
{
    Host *host = context;
    ++host->calls; host->target = *target;
    if (host->nested) {
        UmiEditorSourceLocation output;
        UmiEditorNavigationHistorySnapshot snapshot;
        host->guarded = umi_editor_navigation_history_snapshot(host->history, &snapshot) == UMI_STATUS_OK &&
            umi_editor_navigation_history_record(host->history, target) == UMI_STATUS_BUSY &&
            umi_editor_navigation_history_replace_current(host->history, target) == UMI_STATUS_BUSY &&
            umi_editor_navigation_history_go_back(host->history, &output) == UMI_STATUS_BUSY &&
            umi_editor_navigation_history_go_forward(host->history, &output) == UMI_STATUS_BUSY &&
            umi_editor_navigation_history_clear(host->history) == UMI_STATUS_BUSY &&
            UmiEditorNavigationHistoryRecordJump(host->history, target, target) == UMI_STATUS_BUSY &&
            UmiEditorNavigationHistoryTravel(host->history, -1, snapshot.revision, NULL, Apply, host, NULL) == UMI_STATUS_BUSY;
    }
    return host->result;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1]; int failed = 0;
    Host host = {0};
    UmiEditorSourceLocation first, second, output, before, moved;
    UmiEditorNavigationHistorySnapshot snapshot, after;
    CHECK(umi_editor_navigation_history_create(3U, &host.history) == UMI_STATUS_OK);
    CHECK(umi_editor_source_location_initialize(&first, "file:///first.c", 1U, 1U) == UMI_STATUS_OK);
    CHECK(umi_editor_source_location_initialize(&second, "file:///second.c", 3U, 2U) == UMI_STATUS_OK);
    CHECK(UmiEditorNavigationHistoryRecordJump(host.history, &first, &second) == UMI_STATUS_OK);
    CHECK(umi_editor_navigation_history_snapshot(host.history, &snapshot) == UMI_STATUS_OK && snapshot.count == 2U);
    memset(&output, 0x5a, sizeof(output)); before = output;
    if (strcmp(name, "failure") == 0) {
        host.result = UMI_STATUS_IO_ERROR;
        CHECK(UmiEditorNavigationHistoryTravel(host.history, -1, snapshot.revision, NULL, Apply, &host, &output) == UMI_STATUS_IO_ERROR);
        CHECK(host.calls == 1U && memcmp(&output, &before, sizeof(output)) == 0);
        CHECK(umi_editor_navigation_history_snapshot(host.history, &after) == UMI_STATUS_OK);
        CHECK(after.revision == snapshot.revision && after.current_index == snapshot.current_index);
        host.result = UMI_STATUS_OK;
        CHECK(UmiEditorNavigationHistoryTravel(host.history, -1, snapshot.revision, NULL, Apply, &host, &output) == UMI_STATUS_OK);
        CHECK(umi_editor_source_location_same_position(&output, &first));
    } else if (strcmp(name, "stale-invalid") == 0) {
        CHECK(UmiEditorNavigationHistoryTravel(host.history, -1, snapshot.revision - 1U, NULL, Apply, &host, &output) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiEditorNavigationHistoryTravel(host.history, 0, snapshot.revision, NULL, Apply, &host, &output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiEditorNavigationHistoryTravel(host.history, -1, snapshot.revision, NULL, NULL, &host, &output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(host.calls == 0U && memcmp(&before, &output, sizeof(output)) == 0);
        moved = second; memset(moved.uri, 'x', sizeof(moved.uri));
        CHECK(UmiEditorNavigationHistoryRecordJump(host.history, &first, &moved) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_editor_navigation_history_snapshot(host.history, &after) == UMI_STATUS_OK && after.revision == snapshot.revision);
    } else if (strcmp(name, "departure") == 0 || strcmp(name, "nested") == 0) {
        CHECK(umi_editor_source_location_initialize(&moved, "file:///second.c", 9U, 1U) == UMI_STATUS_OK);
        host.nested = strcmp(name, "nested") == 0;
        CHECK(UmiEditorNavigationHistoryTravel(host.history, -1, snapshot.revision, &moved, Apply, &host, NULL) == UMI_STATUS_OK);
        CHECK(!host.nested || host.guarded);
        CHECK(umi_editor_navigation_history_snapshot(host.history, &after) == UMI_STATUS_OK);
        host.nested = 0;
        CHECK(UmiEditorNavigationHistoryTravel(host.history, 1, after.revision, NULL, Apply, &host, &output) == UMI_STATUS_OK);
        CHECK(umi_editor_source_location_same_position(&output, &moved));
    } else if (strcmp(name, "branch-capacity") == 0) {
        CHECK(umi_editor_source_location_initialize(&moved, "file:///third.c", 1U, 1U) == UMI_STATUS_OK);
        CHECK(UmiEditorNavigationHistoryRecordJump(host.history, &second, &moved) == UMI_STATUS_OK);
        CHECK(UmiEditorNavigationHistoryRecordJump(host.history, &moved, &first) == UMI_STATUS_OK);
        CHECK(umi_editor_navigation_history_snapshot(host.history, &after) == UMI_STATUS_OK && after.count == 3U);
        CHECK(umi_editor_navigation_history_at(host.history, 0U, &output) == UMI_STATUS_OK && umi_editor_source_location_same_position(&output, &second));
        CHECK(UmiEditorNavigationHistoryTravel(host.history, -1, after.revision, NULL, Apply, &host, NULL) == UMI_STATUS_OK);
        CHECK(UmiEditorNavigationHistoryRecordJump(host.history, &moved, &second) == UMI_STATUS_OK);
        CHECK(umi_editor_navigation_history_snapshot(host.history, &after) == UMI_STATUS_OK && !after.can_go_forward);
    } else if (strcmp(name, "same-position") == 0) {
        CHECK(UmiEditorNavigationHistoryRecordJump(host.history, &second, &second) == UMI_STATUS_OK);
        CHECK(umi_editor_navigation_history_snapshot(host.history, &after) == UMI_STATUS_OK && after.revision == snapshot.revision);
    } else { failed = 2; }
cleanup:
    umi_editor_navigation_history_destroy(host.history);
    return failed;
}
