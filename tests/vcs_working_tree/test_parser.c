/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_working_tree/test_parser.c
 * PURPOSE: Cover real Git status record shapes, filename framing and atomic rejection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/vcs/working_tree_review.h"

/* Invalid observations must leave a previously owned result pointer untouched. */
static void Reject(const char *bytes, size_t length, UmiStatus expected)
{
    UmiVcsWorkingTree *sentinel = ParseFixture(NULL, 0U);
    UmiVcsWorkingTree *output = sentinel;
    CHECK(UmiVcsWorkingTreeParse(bytes, length, &output) == expected);
    CHECK(output == sentinel);
    UmiVcsWorkingTreeDestroy(sentinel);
}

int main(int argc, char **argv)
{
    const char *mode;
    UmiVcsWorkingTree *tree = NULL;
    UmiVcsWorkingTreeSummary summary;
    if (argc != 2)
        return 2;
    mode = argv[1];
    if (strcmp(mode, "empty") == 0)
    {
        tree = ParseFixture(NULL, 0U);
        CHECK(UmiVcsWorkingTreeDescribe(tree, &summary) == UMI_STATUS_OK);
        CHECK(!summary.branch_known && summary.entries == 0U);
    }
    else if (strcmp(mode, "headers") == 0)
    {
        static const char input[] =
            HEADERS "# branch.upstream origin/main\0# branch.ab +12 -7\0# stash 3\0";
        tree = ParseFixture(input, sizeof(input) - 1U);
        CHECK(UmiVcsWorkingTreeDescribe(tree, &summary) == UMI_STATUS_OK);
        CHECK(summary.branch_known && summary.commit_known && summary.divergence_known);
        CHECK(summary.ahead == 12U && summary.behind == 7U && summary.stashes == 3U &&
              summary.stash_known);
        CHECK(strcmp(summary.upstream, "origin/main") == 0);
    }
    else if (strcmp(mode, "unknown-divergence") == 0)
    {
        static const char input[] = HEADERS "# branch.upstream origin/main\0# branch.ab +? -?\0";
        tree = ParseFixture(input, sizeof(input) - 1U);
        CHECK(UmiVcsWorkingTreeDescribe(tree, &summary) == UMI_STATUS_OK);
        CHECK(!summary.divergence_known && summary.upstream[0] != '\0');
    }
    else if (strcmp(mode, "unborn") == 0)
    {
        static const char input[] = "# branch.oid (initial)\0# branch.head topic\0? new file.c\0";
        tree = ParseFixture(input, sizeof(input) - 1U);
        CHECK(UmiVcsWorkingTreeDescribe(tree, &summary) == UMI_STATUS_OK);
        CHECK(summary.unborn && !summary.commit_known && summary.untracked == 1U &&
              summary.staged == 0U);
    }
    else if (strcmp(mode, "detached") == 0)
    {
        static const char input[] = "# branch.oid " OBJECT_WIDE "\0# branch.head (detached)\0";
        tree = ParseFixture(input, sizeof(input) - 1U);
        CHECK(UmiVcsWorkingTreeDescribe(tree, &summary) == UMI_STATUS_OK);
        CHECK(summary.detached && strlen(summary.commit_id) == 64U);
    }
    else if (strcmp(mode, "ordinary") == 0)
    {
        static const char input[] = HEADERS ORDINARY "source.c\0";
        tree = ParseFixture(input, sizeof(input) - 1U);
        const UmiVcsWorkingTreeEntry *entry = UmiVcsWorkingTreeEntryAt(tree, 0U);
        CHECK(entry != NULL && entry->change.staged &&
              entry->change.index_state == UMI_VCS_CHANGE_MODIFIED);
        CHECK(entry->change.worktree_state == UMI_VCS_CHANGE_UNMODIFIED && !entry->submodule);
    }
    else if (strcmp(mode, "child") == 0 || strcmp(mode, "child-commit") == 0)
    {
        static const char dirty[] = HEADERS CHILD "framework\0";
        static const char changed[] =
            HEADERS "1 MM SCMU 160000 160000 160000 " OBJECT_ID " " OBJECT_ID " framework\0";
        tree = strcmp(mode, "child") == 0 ? ParseFixture(dirty, sizeof(dirty) - 1U)
                                          : ParseFixture(changed, sizeof(changed) - 1U);
        const UmiVcsWorkingTreeEntry *entry = UmiVcsWorkingTreeEntryAt(tree, 0U);
        CHECK(entry->submodule && entry->child_tracked_changed && entry->child_untracked);
        CHECK(entry->child_commit_changed == (strcmp(mode, "child-commit") == 0));
        CHECK(UmiVcsWorkingTreeDescribe(tree, &summary) == UMI_STATUS_OK);
        CHECK(summary.dirty_children == 1U && summary.staged == (size_t)entry->change.staged);
    }
    else if (strcmp(mode, "rename") == 0 || strcmp(mode, "copy") == 0)
    {
        static const char renamed[] = "2 R. N... 100644 100644 100644 " OBJECT_ID " " OBJECT_ID
                                      " R100 new -> \"file\"\nname.c\0old\tfile.c\0";
        static const char copied[] = "2 C. N... 100644 100644 100644 " OBJECT_WIDE " " OBJECT_WIDE
                                     " C75 copied.c\0original.c\0";
        tree = strcmp(mode, "rename") == 0 ? ParseFixture(renamed, sizeof(renamed) - 1U)
                                           : ParseFixture(copied, sizeof(copied) - 1U);
        const UmiVcsWorkingTreeEntry *entry = UmiVcsWorkingTreeEntryAt(tree, 0U);
        CHECK(strcmp(entry->change.original_path,
                     strcmp(mode, "rename") == 0 ? "old\tfile.c" : "original.c") == 0);
        CHECK(entry->similarity == (strcmp(mode, "rename") == 0 ? 100U : 75U));
        CHECK(entry->change.staged);
    }
    else if (strcmp(mode, "conflict") == 0)
    {
        static const char input[] = "u UU N... 100644 100644 100644 100644 " OBJECT_ID " " OBJECT_ID
                                    " " OBJECT_ID " conflict.c\0";
        tree = ParseFixture(input, sizeof(input) - 1U);
        CHECK(UmiVcsWorkingTreeDescribe(tree, &summary) == UMI_STATUS_OK);
        CHECK(summary.conflicts == 1U && summary.staged == 0U);
        CHECK(UmiVcsWorkingTreeEntryAt(tree, 0U)->unmerged);
    }
    else if (strcmp(mode, "unknown-header") == 0)
    {
        static const char input[] = HEADERS "# future extension value\0? café.c\0! ignored\0";
        tree = ParseFixture(input, sizeof(input) - 1U);
        CHECK(UmiVcsWorkingTreeDescribe(tree, &summary) == UMI_STATUS_OK);
        CHECK(summary.entries == 2U && summary.untracked == 1U);
    }
    else if (strcmp(mode, "path-boundary") == 0)
    {
        char input[UMI_VCS_PATH_CAPACITY + 3U];
        memset(input, 'x', sizeof(input));
        input[0] = '?';
        input[1] = ' ';
        input[UMI_VCS_PATH_CAPACITY + 1U] = '\0';
        tree = ParseFixture(input, UMI_VCS_PATH_CAPACITY + 2U);
        input[UMI_VCS_PATH_CAPACITY + 1U] = 'x';
        input[UMI_VCS_PATH_CAPACITY + 2U] = '\0';
        Reject(input, sizeof(input), UMI_STATUS_CAPACITY_EXCEEDED);
    }
    else if (strcmp(mode, "record-boundary") == 0)
    {
        size_t capacity = (UMI_VCS_MAX_CHANGES + 1U) * 32U, used = 0U;
        char *input = malloc(capacity);
        CHECK(input != NULL);
        for (size_t index = 0U; index < UMI_VCS_MAX_CHANGES; ++index)
        {
            int count = snprintf(input + used, capacity - used, "? path-%zu", index);
            CHECK(count > 0 && (size_t)count + 1U < capacity - used);
            used += (size_t)count + 1U;
        }
        tree = ParseFixture(input, used);
        memcpy(input + used, "? extra\0", 8U);
        Reject(input, used + 8U, UMI_STATUS_CAPACITY_EXCEEDED);
        free(input);
    }
    else
    {
        struct InvalidFixture
        {
            const char *name;
            const char *bytes;
            size_t length;
            UmiStatus status;
        };
#define INVALID(name, bytes, status)                                                               \
    {                                                                                              \
        name, bytes, sizeof(bytes) - 1U, status                                                    \
    }
        static const struct InvalidFixture invalid[] = {
            INVALID("missing-nul", "? file", UMI_STATUS_PARSE_ERROR),
            INVALID("empty-record", "\0", UMI_STATUS_PARSE_ERROR),
            INVALID("missing-source",
                    "2 R. N... 100644 100644 100644 " OBJECT_ID " " OBJECT_ID " R100 target\0",
                    UMI_STATUS_PARSE_ERROR),
            INVALID("duplicate-path", "? file\0? file\0", UMI_STATUS_ALREADY_EXISTS),
            INVALID("duplicate-header", HEADERS "# branch.head other\0", UMI_STATUS_PARSE_ERROR),
            INVALID("empty-header", "# branch.head \0", UMI_STATUS_PARSE_ERROR),
            INVALID("unknown-record", "x data\0", UMI_STATUS_PARSE_ERROR),
            INVALID("bad-mode", "1 M. N... 100648 100644 100644 " OBJECT_ID " " OBJECT_ID " file\0",
                    UMI_STATUS_PARSE_ERROR),
            INVALID("bad-object", "1 M. N... 100644 100644 100644 short " OBJECT_ID " file\0",
                    UMI_STATUS_PARSE_ERROR),
            INVALID("bad-child",
                    "1 .M S.CU 160000 160000 160000 " OBJECT_ID " " OBJECT_ID " child\0",
                    UMI_STATUS_PARSE_ERROR),
            INVALID("bad-code", "1 Q. N... 100644 100644 100644 " OBJECT_ID " " OBJECT_ID " file\0",
                    UMI_STATUS_PARSE_ERROR),
            INVALID("bad-score",
                    "2 R. N... 100644 100644 100644 " OBJECT_ID " " OBJECT_ID
                    " R101 target\0source\0",
                    UMI_STATUS_PARSE_ERROR),
            INVALID("bad-conflict",
                    "u MM N... 100644 100644 100644 100644 " OBJECT_ID " " OBJECT_ID " " OBJECT_ID
                    " file\0",
                    UMI_STATUS_PARSE_ERROR),
            INVALID("divergence-no-upstream", "# branch.ab +1 -2\0", UMI_STATUS_PARSE_ERROR),
            INVALID("overflow", "# stash 9999999999999999999999999999999999999999\0",
                    UMI_STATUS_CAPACITY_EXCEEDED),
        };
#undef INVALID
        int found = 0;
        for (size_t index = 0U; index < sizeof(invalid) / sizeof(invalid[0]); ++index)
            if (strcmp(mode, invalid[index].name) == 0)
            {
                Reject(invalid[index].bytes, invalid[index].length, invalid[index].status);
                found = 1;
            }
        CHECK(found);
    }
    if (tree != NULL)
        CHECK(UmiVcsWorkingTreeEntryAt(tree, UMI_VCS_MAX_CHANGES + 1U) == NULL);
    UmiVcsWorkingTreeDestroy(tree);
    return 0;
}
