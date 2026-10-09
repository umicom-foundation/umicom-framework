/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_working_tree/test_review.c
 * PURPOSE: Check child commit explanations, escaped display names and atomic legacy publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/vcs/working_tree_review.h"
#include <limits.h>

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    static const char input[] = HEADERS CHILD "framework\0" ORDINARY "line\nname.c\0";
    UmiVcsWorkingTree *tree = ParseFixture(input, sizeof(input) - 1U);
    char *text = malloc(UMI_VCS_WORKING_TREE_ENTRY_TEXT_CAPACITY);
    CHECK(text != NULL);
    if (strcmp(mode, "summary") == 0)
    {
        CHECK(UmiVcsWorkingTreeSummaryText(tree, text, UMI_VCS_WORKING_TREE_ENTRY_TEXT_CAPACITY) ==
              UMI_STATUS_OK);
        CHECK(strstr(text, "1 staged") && strstr(text, "1 children with uncommitted") &&
              strstr(text, "does not include those files"));
    }
    else if (strcmp(mode, "child-only") == 0)
    {
        UmiVcsWorkingTreeDestroy(tree);
        static const char child[] = HEADERS CHILD "framework\0";
        tree = ParseFixture(child, sizeof(child) - 1U);
        CHECK(UmiVcsWorkingTreeSummaryText(tree, text, UMI_VCS_WORKING_TREE_ENTRY_TEXT_CAPACITY) ==
              UMI_STATUS_OK);
        CHECK(strstr(text, "Nothing is staged here") && strstr(text, "inside each child"));
    }
    else if (strcmp(mode, "escaped-path") == 0)
    {
        CHECK(UmiVcsWorkingTreeEntryText(
                  tree, 1U, text, UMI_VCS_WORKING_TREE_ENTRY_TEXT_CAPACITY) == UMI_STATUS_OK);
        CHECK(strstr(text, "line\\x0aname.c") && !strstr(text, "line\nname.c"));
    }
    else if (strcmp(mode, "child-advice") == 0)
    {
        CHECK(UmiVcsWorkingTreeEntryText(
                  tree, 0U, text, UMI_VCS_WORKING_TREE_ENTRY_TEXT_CAPACITY) == UMI_STATUS_OK);
        CHECK(strstr(text, "tracked file changes") && strstr(text, "untracked files") &&
              !strstr(text, "differs from the parent's index"));
    }
    else if (strcmp(mode, "small-buffer") == 0)
    {
        memcpy(text, "keep", 5U);
        CHECK(UmiVcsWorkingTreeSummaryText(tree, text, 5U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(text, "keep") == 0);
        CHECK(UmiVcsWorkingTreeEntryText(tree, 0U, text, 5U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(text, "keep") == 0);
        CHECK(UmiVcsWorkingTreeEntryText(tree, 99U, text, 5U) == UMI_STATUS_NOT_FOUND);
        CHECK(strcmp(text, "keep") == 0);
    }
    else if (strcmp(mode, "legacy") == 0 || strcmp(mode, "legacy-overflow") == 0)
    {
        UmiVcsChangeList *list = NULL;
        UmiVcsBranch branch = {0};
        UmiVcsChange old = {0};
        strcpy(old.path, "old.c");
        strcpy(branch.name, "old-branch");
        CHECK(umi_vcs_change_list_create(&list) == UMI_STATUS_OK);
        CHECK(umi_vcs_change_list_add(list, &old) == UMI_STATUS_OK);
        if (strcmp(mode, "legacy") == 0)
        {
            CHECK(UmiVcsWorkingTreeCopyChanges(tree, list, &branch) == UMI_STATUS_OK);
            CHECK(strcmp(branch.name, "main") == 0 && umi_vcs_change_list_count(list) == 2U);
            CHECK(umi_vcs_change_list_find(list, "old.c") == NULL);
            CHECK(umi_vcs_change_list_find(list, "line\nname.c") != NULL);
        }
        else
        {
            char counters[256];
            UmiVcsWorkingTreeDestroy(tree);
            int count = snprintf(counters, sizeof(counters), "# branch.upstream origin/main");
            CHECK(count > 0);
            size_t length = (size_t)count + 1U;
            count = snprintf(counters + length, sizeof(counters) - length, "# branch.ab +%zu -0",
                             (size_t)INT_MAX + 1U);
            CHECK(count > 0);
            tree = ParseFixture(counters, length + (size_t)count + 1U);
            CHECK(UmiVcsWorkingTreeCopyChanges(tree, list, &branch) ==
                  UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(strcmp(branch.name, "old-branch") == 0 &&
                  umi_vcs_change_list_find(list, "old.c") != NULL);
        }
        umi_vcs_change_list_destroy(list);
    }
    else
        CHECK(0);
    free(text);
    UmiVcsWorkingTreeDestroy(tree);
    return 0;
}
