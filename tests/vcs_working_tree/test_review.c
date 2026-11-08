/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_working_tree/test_review.c
 *
 * PURPOSE:
 *   Verify repository identity display, child explanations and atomic rendering boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/vcs/working_tree_review.h"
#include <limits.h>

/* Exercise publication boundaries against the complete previously rendered
 * message. A rejected call must preserve every caller byte, and a successful
 * exact-size call must leave the following guard byte and raw Git paths alone. */
static void CheckEntryCapacity(const UmiVcsWorkingTree *tree, size_t index, char *text)
{
    const size_t capacity = UMI_VCS_WORKING_TREE_ENTRY_TEXT_CAPACITY;
    const UmiVcsWorkingTreeEntry *entry = UmiVcsWorkingTreeEntryAt(tree, index);
    CHECK(entry != NULL);
    UmiVcsWorkingTreeEntry saved;
    memcpy(&saved, entry, sizeof(saved));
    char *expected = malloc(capacity);
    CHECK(expected != NULL);
    CHECK(UmiVcsWorkingTreeEntryText(tree, index, expected, capacity) == UMI_STATUS_OK);
    const size_t required = strlen(expected) + 1U;
    CHECK(required > 1U && required < capacity);
    memset(text, '~', capacity);
    CHECK(UmiVcsWorkingTreeEntryText(tree, index, text, required - 1U) ==
          UMI_STATUS_CAPACITY_EXCEEDED);
    for (size_t byte = 0U; byte < capacity; ++byte)
        CHECK(text[byte] == '~');
    CHECK(UmiVcsWorkingTreeEntryText(tree, index, text, required) == UMI_STATUS_OK);
    CHECK(memcmp(text, expected, required) == 0 && text[required] == '~');
    CHECK(memcmp(entry, &saved, sizeof(saved)) == 0);
    free(expected);
}

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
    else if (!strcmp(mode, "identity") || !strcmp(mode, "identity-wide") ||
        !strcmp(mode, "identity-unborn") || !strcmp(mode, "identity-unknown") ||
        !strcmp(mode, "identity-detached") || !strcmp(mode, "identity-upstream") ||
        !strcmp(mode, "identity-maximum"))
    {
        UmiVcsWorkingTreeDestroy(tree);
        static const char wide[] = "# branch.oid " OBJECT_WIDE "\0# branch.head main\0";
        static const char unborn[] = "# branch.oid (initial)\0# branch.head main\0";
        static const char detached[] = "# branch.oid " OBJECT_ID "\0# branch.head (detached)\0";
        static const char upstream[] = HEADERS "# branch.upstream origin/line\nname\0# branch.ab +2 -3\0# stash 4\0";
        if (!strcmp(mode, "identity-wide")) tree = ParseFixture(wide, sizeof(wide) - 1U);
        else if (!strcmp(mode, "identity-unborn")) tree = ParseFixture(unborn, sizeof(unborn) - 1U);
        else if (!strcmp(mode, "identity-unknown")) tree = ParseFixture(NULL, 0U);
        else if (!strcmp(mode, "identity-detached")) tree = ParseFixture(detached, sizeof(detached) - 1U);
        else if (!strcmp(mode, "identity-upstream")) tree = ParseFixture(upstream, sizeof(upstream) - 1U);
        else if (!strcmp(mode, "identity-maximum")) {
            char bytes[2U * UMI_VCS_NAME_CAPACITY + 128U]; size_t used = 0U;
            const char *prefixes[] = {"# branch.head ", "# branch.upstream "};
            for (size_t n = 0U; n < 2U; ++n) {
                size_t prefix = strlen(prefixes[n]); memcpy(bytes + used, prefixes[n], prefix); used += prefix;
                memset(bytes + used, n ? 127 : 1, UMI_VCS_NAME_CAPACITY - 1U);
                used += UMI_VCS_NAME_CAPACITY - 1U; bytes[used++] = '\0';
            }
            tree = ParseFixture(bytes, used);
        } else tree = ParseFixture(input, sizeof(input) - 1U);
        CHECK(UmiVcsWorkingTreeSummaryText(tree, text, UMI_VCS_WORKING_TREE_SUMMARY_TEXT_CAPACITY) == UMI_STATUS_OK);
        if (!strcmp(mode, "identity-wide")) CHECK(strstr(text, "Commit (HEAD): " OBJECT_WIDE));
        else if (!strcmp(mode, "identity-unborn")) CHECK(strstr(text, "Commit (HEAD): (unborn)"));
        else if (!strcmp(mode, "identity-unknown")) CHECK(strstr(text, "Commit (HEAD): (not reported)"));
        else if (!strcmp(mode, "identity-maximum")) CHECK(strstr(text, "\\x01\\x01") && strstr(text, "\\x7f\\x7f"));
        else CHECK(strstr(text, "Commit (HEAD): " OBJECT_ID));
        if (!strcmp(mode, "identity-upstream")) {
            CHECK(strstr(text, "Upstream: \"origin/line\\x0aname\""));
            CHECK(strstr(text, "2 ahead, 3 behind") && strstr(text, "Stashes: 4"));
        } else CHECK(strstr(text, "Stashes: not reported"));
        if (!strcmp(mode, "identity-detached")) CHECK(strstr(text, "(detached HEAD)"));
        size_t required = strlen(text) + 1U;
        char saved[UMI_VCS_WORKING_TREE_SUMMARY_TEXT_CAPACITY]; memcpy(saved, text, required);
        memset(text, '~', UMI_VCS_WORKING_TREE_ENTRY_TEXT_CAPACITY);
        CHECK(UmiVcsWorkingTreeSummaryText(tree, text, required - 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
        for (size_t i = 0U; i < UMI_VCS_WORKING_TREE_ENTRY_TEXT_CAPACITY; ++i) CHECK(text[i] == '~');
        CHECK(UmiVcsWorkingTreeSummaryText(tree, text, required) == UMI_STATUS_OK);
        CHECK(!memcmp(text, saved, required) && text[required] == '~');
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
    else if (strcmp(mode, "renamed-path") == 0)
    {
        /* Expected display text is independent of the production escape helper.
         * Both source operands are nonempty, and neither may change in the tree. */
        static const char renamed[] =
            "2 R. N... 100644 100644 100644 " OBJECT_ID " " OBJECT_ID
            " R100 new -> \"file\"\nname.c\0old\\dir\tfile.c\0";
        static const char expected[] =
            "\"new -> \\\"file\\\"\\x0aname.c\"\nIndex: R; worktree: .. Staged change.\n"
            "Original path: \"old\\\\dir\\x09file.c\"\n";
        UmiVcsWorkingTreeDestroy(tree);
        tree = ParseFixture(renamed, sizeof(renamed) - 1U);
        CheckEntryCapacity(tree, 0U, text);
        CHECK(strcmp(text, expected) == 0);
    }
    else if (strcmp(mode, "maximum-paths") == 0)
    {
        /* Two maximum-length names both expand fourfold. This reaches each
         * scratch-name boundary while checking the complete message and its
         * publication limit without relying on ordinary printable paths. */
        static const char prefix[] =
            "2 R. N... 100644 100644 100644 " OBJECT_ID " " OBJECT_ID " R100 ";
        static const char middle[] =
            "\"\nIndex: R; worktree: .. Staged change.\nOriginal path: \"";
        const size_t prefixLength = sizeof(prefix) - 1U;
        const size_t pathLength = UMI_VCS_PATH_CAPACITY - 1U;
        const size_t inputLength = prefixLength + 2U * (pathLength + 1U);
        char *boundaryInput = malloc(inputLength);
        CHECK(boundaryInput != NULL);
        memcpy(boundaryInput, prefix, prefixLength);
        memset(boundaryInput + prefixLength, 1, pathLength);
        boundaryInput[prefixLength + pathLength] = '\0';
        memset(boundaryInput + prefixLength + pathLength + 1U, 127, pathLength);
        boundaryInput[inputLength - 1U] = '\0';
        UmiVcsWorkingTreeDestroy(tree);
        tree = ParseFixture(boundaryInput, inputLength);
        free(boundaryInput);
        CheckEntryCapacity(tree, 0U, text);
        CHECK(text[0] == '"');
        size_t textOffset = 1U;
        for (size_t byte = 0U; byte < pathLength; ++byte, textOffset += 4U)
            CHECK(memcmp(text + textOffset, "\\x01", 4U) == 0);
        CHECK(memcmp(text + textOffset, middle, sizeof(middle) - 1U) == 0);
        textOffset += sizeof(middle) - 1U;
        for (size_t byte = 0U; byte < pathLength; ++byte, textOffset += 4U)
            CHECK(memcmp(text + textOffset, "\\x7f", 4U) == 0);
        CHECK(strcmp(text + textOffset, "\"\n") == 0);
    }
    else if (strcmp(mode, "exact-buffer") == 0)
    {
        CheckEntryCapacity(tree, 0U, text);
        CheckEntryCapacity(tree, 1U, text);
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
