/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/watch_edit/test_registry.c
 * PURPOSE: Keep registry proposals and retained workspace edit handles independent.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Include the registry contract before the workspace fixture. Existing edit
 * tests include the workspace contract first, covering both public use paths. */
#include "umicom/debug/watch.h"
#include "fixture.h"

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const int remove_item = strcmp(argv[1], "remove") == 0;
    CHECK(remove_item || strcmp(argv[1], "upsert") == 0);
    Fixture fixture = {0};
    Open(&fixture);
    UmiDebugWatchEdit *handle = NULL;
    OK(UmiDebugWatchEditCapture(fixture.workspace, 0U, &handle));
    UmiDebugWatchSnapshot before = Read(&fixture);
    UmiDebugWatchRegistryEdit proposal = {0};
    proposal.kind = remove_item ? UMI_SNAPSHOT_EDIT_REMOVE : UMI_SNAPSHOT_EDIT_UPSERT;
    proposal.item = before;
    if (!remove_item) strcpy(proposal.item.expression, "registry proposal");
    UmiSnapshotBatchResult published = {0};
    uint64_t revision = umi_debug_watch_registry_revision(fixture.registry);
    OK(umi_debug_watch_registry_edit_if_current(fixture.registry, revision,
        &proposal, 1U, &published));
    CHECK(published.applied == 1U);
    CHECK(umi_debug_watch_registry_revision(fixture.registry) == revision + 1U);

    /* A registry edit changes the owner's revision. The old workspace handle
     * must refuse publication while retaining its independently copied row. */
    CHECK(UmiDebugWatchEditValidate(fixture.workspace, handle) == UMI_STATUS_BUSY);
    UmiDebugWatchSnapshot captured = {0};
    OK(UmiDebugWatchEditRead(handle, &captured));
    CHECK(strcmp(captured.id, before.id) == 0);
    CHECK(strcmp(captured.expression, before.expression) == 0);
    UmiDebugWatchSettings settings;
    OK(UmiDebugWatchSettingsInit(&settings, 1, "afterPublication"));
    UmiDebugWatchChange change;
    CHECK(UmiDebugWatchEditApply(fixture.workspace, handle, &settings, &change) == UMI_STATUS_BUSY);
    CHECK(!change.changed && !change.removed);
    UmiDebugWatchEditDestroy(handle);
    handle = NULL;
    if (remove_item) {
        CHECK(umi_debug_watch_registry_count(fixture.registry) == 0U);
        CHECK(UmiDebugWatchEditCapture(fixture.workspace, 0U, &handle) == UMI_STATUS_NOT_FOUND);
        CHECK(handle == NULL);
    } else {
        CHECK(strcmp(Read(&fixture).expression, "registry proposal") == 0);
        /* A fresh handle uses the unchanged workspace API and can still apply
         * a real property edit after the reviewed registry publication. */
        OK(UmiDebugWatchEditCapture(fixture.workspace, 0U, &handle));
        OK(UmiDebugWatchEditApply(fixture.workspace, handle, &settings, &change));
        CHECK(change.changed);
        CHECK(strcmp(Read(&fixture).expression, settings.expression) == 0);
        UmiDebugWatchEditDestroy(handle);
    }
    Close(&fixture);
    return 0;
}
