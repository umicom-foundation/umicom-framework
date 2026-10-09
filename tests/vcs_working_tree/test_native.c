/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_working_tree/test_native.c
 * PURPOSE: Check actual Git observations of child files, child commits and staged parent pointers.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"

int main(int argc, char **argv)
{
    if (argc != 4)
        return 2;
    UmiVcsWorkingTreeRequest request = {0};
    UmiVcsWorkingTree *tree = NULL;
    UmiVcsWorkingTreeSummary summary;
    request.repository_root = argv[1];
    request.git_program = argv[3];
    CHECK(UmiVcsWorkingTreeRead(&request, &tree) == UMI_STATUS_OK);
    CHECK(UmiVcsWorkingTreeDescribe(tree, &summary) == UMI_STATUS_OK);
    CHECK(summary.branch_known && strcmp(summary.branch, "main") == 0);
    const char *mode = argv[2];
    if (strcmp(mode, "unborn") == 0)
        CHECK(summary.unborn && summary.entries == 0U);
    else if (strcmp(mode, "clean") == 0)
        CHECK(summary.commit_known && summary.entries == 0U);
    else
    {
        CHECK(summary.entries == 1U);
        const UmiVcsWorkingTreeEntry *entry = UmiVcsWorkingTreeEntryAt(tree, 0U);
        CHECK(entry && entry->submodule && strcmp(entry->change.path, "framework") == 0);
        if (strcmp(mode, "child-dirty") == 0)
        {
            CHECK(summary.staged == 0U && summary.dirty_children == 1U);
            CHECK(entry->child_tracked_changed && entry->child_untracked &&
                  !entry->child_commit_changed);
        }
        else if (strcmp(mode, "child-committed") == 0)
        {
            CHECK(summary.staged == 0U && summary.dirty_children == 0U);
            CHECK(entry->child_commit_changed);
        }
        else if (strcmp(mode, "parent-staged") == 0)
        {
            CHECK(summary.staged == 1U && summary.dirty_children == 0U);
            CHECK(!entry->child_commit_changed);
        }
        else if (strcmp(mode, "parent-staged-child-dirty") == 0)
        {
            CHECK(summary.staged == 1U && summary.dirty_children == 1U);
            CHECK(entry->child_tracked_changed && !entry->child_commit_changed);
        }
        else
            CHECK(0);
    }
    UmiVcsWorkingTreeDestroy(tree);
    return 0;
}
