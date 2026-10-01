/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/breakpoint_edit/test_registry.c
 * PURPOSE: Keep registry proposals and retained workspace edit handles independent.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Include the registry contract before the workspace fixture. Existing edit
 * tests include the workspace contract first, covering both public use paths. */
#include "umicom/debug/breakpoint.h"
#include "fixture.h"

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const int remove_item = strcmp(argv[1], "remove") == 0;
    CHECK(remove_item || strcmp(argv[1], "upsert") == 0);
    Fixture fixture = {0};
    Open(&fixture);
    UmiDebugBreakpointEdit *handle = NULL;
    OK(UmiDebugBreakpointEditCapture(fixture.workspace, 0U, &handle));
    UmiDebugBreakpointSnapshot before = Read(&fixture);
    UmiDebugBreakpointRegistryEdit proposal = {0};
    proposal.kind = remove_item ? UMI_SNAPSHOT_EDIT_REMOVE : UMI_SNAPSHOT_EDIT_UPSERT;
    proposal.item = before;
    if (!remove_item) strcpy(proposal.item.condition, "registry proposal");
    UmiSnapshotBatchResult published = {0};
    uint64_t revision = umi_debug_breakpoint_registry_revision(fixture.registry);
    OK(umi_debug_breakpoint_registry_edit_if_current(fixture.registry, revision,
        &proposal, 1U, &published));
    CHECK(published.applied == 1U);
    CHECK(umi_debug_breakpoint_registry_revision(fixture.registry) == revision + 1U);

    /* A registry edit changes the owner's revision. The old workspace handle
     * must refuse publication while retaining its independently copied row. */
    CHECK(UmiDebugBreakpointEditValidate(fixture.workspace, handle) == UMI_STATUS_BUSY);
    UmiDebugBreakpointSnapshot captured = {0};
    OK(UmiDebugBreakpointEditRead(handle, &captured));
    CHECK(strcmp(captured.id, before.id) == 0);
    CHECK(strcmp(captured.condition, before.condition) == 0);
    UmiDebugBreakpointSettings settings;
    OK(UmiDebugBreakpointSettingsInit(&settings, 1, "after publication", ""));
    UmiDebugBreakpointChange change;
    CHECK(UmiDebugBreakpointEditApply(fixture.workspace, handle, &settings, &change) == UMI_STATUS_BUSY);
    CHECK(!change.changed && !change.removed);
    UmiDebugBreakpointEditDestroy(handle);
    handle = NULL;
    if (remove_item) {
        CHECK(umi_debug_breakpoint_registry_count(fixture.registry) == 0U);
        CHECK(UmiDebugBreakpointEditCapture(fixture.workspace, 0U, &handle) == UMI_STATUS_NOT_FOUND);
        CHECK(handle == NULL);
    } else {
        CHECK(strcmp(Read(&fixture).condition, "registry proposal") == 0);
        /* A fresh handle uses the unchanged workspace API and can still apply
         * a real property edit after the reviewed registry publication. */
        OK(UmiDebugBreakpointEditCapture(fixture.workspace, 0U, &handle));
        OK(UmiDebugBreakpointEditApply(fixture.workspace, handle, &settings, &change));
        CHECK(change.changed);
        CHECK(strcmp(Read(&fixture).condition, settings.condition) == 0);
        UmiDebugBreakpointEditDestroy(handle);
    }
    Close(&fixture);
    return 0;
}
