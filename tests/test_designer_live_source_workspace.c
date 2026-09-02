/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_designer_live_source_workspace.c
 * PURPOSE: Verify debounced, revision-safe live source preview state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/designer/live_source_workspace.h"

/* Exercise successful, invalid and stale-provider paths without running user code. */
int main(void)
{
    UmiDesignerLiveSourceWorkspace workspace;

    assert(umi_designer_live_source_workspace_init(
        &workspace,
        UMI_DESIGNER_SOURCE_HTML,
        400U) == UMI_STATUS_OK);
    assert(umi_designer_live_source_workspace_set_mode(
        &workspace,
        UMI_DESIGNER_WORKSPACE_MIXED) == UMI_STATUS_OK);
    assert(umi_designer_live_source_workspace_update(
        &workspace,
        "<main>Welcome</main>",
        7U,
        1000U) == UMI_STATUS_OK);
    assert(!umi_designer_live_source_workspace_is_due(&workspace, 1399U));
    assert(umi_designer_live_source_workspace_is_due(&workspace, 1400U));
    assert(umi_designer_live_source_workspace_accept(
        &workspace,
        7U,
        "rendered document") == UMI_STATUS_OK);
    assert(workspace.health == UMI_DESIGNER_PREVIEW_CURRENT);
    assert(strcmp(workspace.preview, "rendered document") == 0);

    assert(umi_designer_live_source_workspace_update(
        &workspace,
        "<main>",
        8U,
        2000U) == UMI_STATUS_OK);
    assert(workspace.health == UMI_DESIGNER_PREVIEW_STALE);
    /* An edit cannot silently reuse a revision that already identifies other
     * source text, and a late renderer cannot publish revision seven now. */
    assert(umi_designer_live_source_workspace_update(
        &workspace,
        "<main>Different text</main>",
        8U,
        2100U) == UMI_STATUS_INVALID_STATE);
    assert(umi_designer_live_source_workspace_accept(
        &workspace,
        7U,
        "late rendered document") == UMI_STATUS_INVALID_STATE);
    assert(umi_designer_live_source_workspace_reject(
        &workspace,
        8U,
        "Closing element is missing.") == UMI_STATUS_OK);
    assert(workspace.health == UMI_DESIGNER_PREVIEW_INVALID);
    /* A failed edit does not erase the last preview that was known to work. */
    assert(strcmp(workspace.preview, "rendered document") == 0);
    assert(umi_designer_source_language_from_path("main.c") ==
        UMI_DESIGNER_SOURCE_C);
    assert(umi_designer_source_language_from_path("page.HTML") ==
        UMI_DESIGNER_SOURCE_HTML);
    assert(strcmp(
        umi_designer_workspace_mode_text(UMI_DESIGNER_WORKSPACE_PREVIEW),
        "preview") == 0);
    return 0;
}
