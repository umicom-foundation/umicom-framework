/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_inspection/test_porcelain_bytes.c
 * PURPOSE: Check complete repository status evidence with unusual filenames and invalid records.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/repository/porcelain_parser.h"
#include "umicom/repository/git_capture.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define HASH "0123456789abcdef0123456789abcdef01234567"
#define WIDE_HASH "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
#define MODES "100644 100644 100644 "
#define OBJECTS HASH " " HASH " "
#define CHECK(value) do { if (!(value)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #value); free(owned); return 1; \
} } while (0)

typedef struct Example {
    const char *name;
    const char *bytes;
    size_t length;
    UmiStatus expected;
    size_t modified, deleted, untracked, conflicts, staged, gitlinks, submodules;
} Example;
#define SAMPLE(name, text, result, m, d, u, c, s, g, sub) \
    {name, text, sizeof(text) - 1U, result, m, d, u, c, s, g, sub}
static const Example examples[] = {
    SAMPLE("clean", "# branch.oid " HASH "\0# branch.head main\0", UMI_STATUS_OK, 0,0,0,0,0,0,0),
    SAMPLE("initial", "# branch.oid (initial)\0# branch.head main\0", UMI_STATUS_OK, 0,0,0,0,0,0,0),
    SAMPLE("detached", "# branch.oid " HASH "\0# branch.head (detached)\0", UMI_STATUS_OK, 0,0,0,0,0,0,0),
    SAMPLE("tracking", "# branch.head main\0# branch.upstream origin/main\0# branch.ab +2 -3\0",
        UMI_STATUS_OK, 0,0,0,0,0,0,0),
    SAMPLE("modified", "1 .M N... " MODES OBJECTS "folder/a file.c\0", UMI_STATUS_OK, 1,0,0,0,0,0,0),
    SAMPLE("staged", "1 M. N... " MODES OBJECTS "file.c\0", UMI_STATUS_OK, 0,0,0,0,1,0,0),
    SAMPLE("deleted", "1 .D N... " MODES OBJECTS "file.c\0", UMI_STATUS_OK, 0,1,0,0,0,0,0),
    SAMPLE("rename", "2 R. N... " MODES OBJECTS "R100 new name.c\0? original\nname.c\0",
        UMI_STATUS_OK, 0,0,0,0,1,0,0),
    SAMPLE("copy", "2 C. N... " MODES OBJECTS "C75 new.c\0old.c\0", UMI_STATUS_OK, 0,0,0,0,1,0,0),
    SAMPLE("conflict", "u UU N... 100644 100644 100644 100644 " HASH " " HASH " " HASH " file.c\0",
        UMI_STATUS_OK, 0,0,0,1,0,0,0),
    SAMPLE("clean_submodule", "1 M. S... 160000 160000 160000 " OBJECTS "framework\0",
        UMI_STATUS_OK, 0,0,0,0,1,1,0),
    SAMPLE("dirty_submodule", "1 .M S.MU 160000 160000 160000 " OBJECTS "framework\0",
        UMI_STATUS_OK, 1,0,0,0,0,0,1),
    SAMPLE("changed_gitlink", "1 MM SC.. 160000 160000 160000 " OBJECTS "framework\0",
        UMI_STATUS_OK, 1,0,0,0,1,1,1),
    SAMPLE("gitlink_to_file", "1 T. N... 160000 100644 100644 " OBJECTS "former-child\0",
        UMI_STATUS_OK, 0,0,0,0,1,1,0),
    SAMPLE("file_to_gitlink", "1 T. S... 100644 160000 160000 " OBJECTS "new-child\0",
        UMI_STATUS_OK, 0,0,0,0,1,1,0),
    SAMPLE("worktree_gitlink", "1 .T S... 100644 100644 160000 " OBJECTS "child\0",
        UMI_STATUS_OK, 1,0,0,0,0,0,0),
    SAMPLE("mixed_conflict_hash", "u UU N... 100644 100644 100644 100644 " HASH " " WIDE_HASH " " HASH " file.c\0",
        UMI_STATUS_PARSE_ERROR, 0,0,0,0,0,0,0),
    SAMPLE("untracked", "? spaces and\ttabs\nnewlines.c\0? another.c\0", UMI_STATUS_OK, 0,0,2,0,0,0,0),
    SAMPLE("ignored", "! cache folder/\0", UMI_STATUS_OK, 0,0,0,0,0,0,0),
    SAMPLE("unknown_header", "# future.header future-value\0? file.c\0", UMI_STATUS_OK, 0,0,1,0,0,0,0),
    SAMPLE("sha256", "1 .M N... " MODES WIDE_HASH " " WIDE_HASH " file.c\0",
        UMI_STATUS_OK, 1,0,0,0,0,0,0),
    SAMPLE("truncated", "? incomplete.c", UMI_STATUS_PARSE_ERROR, 0,0,0,0,0,0,0),
    SAMPLE("empty_record", "? first.c\0\0", UMI_STATUS_PARSE_ERROR, 0,0,0,0,0,0,0),
    SAMPLE("unknown_record", "? first.c\0x unknown\0", UMI_STATUS_PARSE_ERROR, 0,0,0,0,0,0,0),
    SAMPLE("diagnostic", "warning: cannot inspect\0", UMI_STATUS_PARSE_ERROR, 0,0,0,0,0,0,0),
    SAMPLE("rename_source_missing", "2 R. N... " MODES OBJECTS "R100 new.c\0",
        UMI_STATUS_PARSE_ERROR, 0,0,0,0,0,0,0),
    SAMPLE("rename_source_empty", "2 R. N... " MODES OBJECTS "R100 new.c\0\0",
        UMI_STATUS_PARSE_ERROR, 0,0,0,0,0,0,0),
    SAMPLE("bad_xy", "1 X. N... " MODES OBJECTS "file.c\0", UMI_STATUS_PARSE_ERROR, 0,0,0,0,0,0,0),
    SAMPLE("bad_submodule", "1 .M SX.. " MODES OBJECTS "file.c\0", UMI_STATUS_PARSE_ERROR, 0,0,0,0,0,0,0),
    SAMPLE("bad_mode", "1 .M N... 100648 100644 100644 " OBJECTS "file.c\0",
        UMI_STATUS_PARSE_ERROR, 0,0,0,0,0,0,0),
    SAMPLE("bad_hash", "1 .M N... " MODES "bad " HASH " file.c\0",
        UMI_STATUS_PARSE_ERROR, 0,0,0,0,0,0,0),
    SAMPLE("bad_score", "2 R. N... " MODES OBJECTS "R101 new.c\0old.c\0",
        UMI_STATUS_PARSE_ERROR, 0,0,0,0,0,0,0),
    SAMPLE("mixed_hash", "1 .M N... " MODES HASH " " WIDE_HASH " file.c\0",
        UMI_STATUS_PARSE_ERROR, 0,0,0,0,0,0,0),
    SAMPLE("ordinary_rename", "1 R. N... " MODES OBJECTS "file.c\0",
        UMI_STATUS_PARSE_ERROR, 0,0,0,0,0,0,0),
    SAMPLE("score_mismatch", "2 R. N... " MODES OBJECTS "C80 new.c\0old.c\0",
        UMI_STATUS_PARSE_ERROR, 0,0,0,0,0,0,0),
    SAMPLE("bad_conflict", "u MM N... 100644 100644 100644 100644 " HASH " " HASH " " HASH " file.c\0",
        UMI_STATUS_PARSE_ERROR, 0,0,0,0,0,0,0),
    SAMPLE("count_overflow", "# branch.ab +99999999999999999999999999 -0\0",
        UMI_STATUS_CAPACITY_EXCEEDED, 0,0,0,0,0,0,0),
    SAMPLE("count_suffix", "# branch.ab +2 -3 extra\0", UMI_STATUS_PARSE_ERROR, 0,0,0,0,0,0,0),
    SAMPLE("duplicate_header", "# branch.head main\0# branch.head other\0",
        UMI_STATUS_PARSE_ERROR, 0,0,0,0,0,0,0),
    SAMPLE("empty_path", "? \0", UMI_STATUS_PARSE_ERROR, 0,0,0,0,0,0,0),
    SAMPLE("missing_fields", "1 .M N... file.c\0", UMI_STATUS_PARSE_ERROR, 0,0,0,0,0,0,0)
};

/* Counts are published only for a complete snapshot. Invalid trailing records
 * must not retain an earlier apparently valid count or branch name. */
int main(int argc, char **argv)
{
    char *owned = NULL;
    CHECK(argc == 2);
    UmiRepositoryPorcelainStatus value;
    if (strcmp(argv[1], "null") == 0) {
        CHECK(umi_repository_porcelain_parse_bytes(NULL, 1U, &value) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_repository_porcelain_parse_bytes(NULL, 0U, &value) == UMI_STATUS_OK);
        CHECK(umi_repository_porcelain_parse_bytes("", 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_repository_porcelain_parse_bytes("", UMI_REPOSITORY_GIT_CAPTURE_LIMIT + 1U,
            &value) == UMI_STATUS_CAPACITY_EXCEEDED);
        return 0;
    }
    if (strcmp(argv[1], "long_path") == 0) {
        owned = malloc(10003U); CHECK(owned != NULL);
        owned[0] = '?'; owned[1] = ' ';
        memset(owned + 2, 'x', 10000U); owned[10002] = '\0';
        /* Preserve the aggregate-only expectation for review. Shared VCS entries
         * deliberately bound exact paths, so maintenance must reject this too. */
#if 0
        CHECK(umi_repository_porcelain_parse_bytes(owned, 10003U, &value) == UMI_STATUS_OK);
        CHECK(value.worktree.untracked == 1U);
#endif
        CHECK(umi_repository_porcelain_parse_bytes(owned, 10003U, &value) ==
            UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(value.worktree.untracked == 0U);
        free(owned); return 0;
    }
    if (strcmp(argv[1], "long_branch") == 0) {
        owned = malloc(400U); CHECK(owned != NULL);
        memcpy(owned, "# branch.head ", 14U); memset(owned + 14, 'x', 300U); owned[314] = '\0';
        CHECK(umi_repository_porcelain_parse_bytes(owned, 315U, &value) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(value.branch.branch[0] == '\0');
        free(owned); return 0;
    }
    const Example *example = NULL;
    for (size_t i = 0U; i < sizeof examples / sizeof examples[0]; ++i)
        if (strcmp(argv[1], examples[i].name) == 0) example = &examples[i];
    CHECK(example != NULL);
    memset(&value, 0x5a, sizeof value);
    CHECK(umi_repository_porcelain_parse_bytes(example->bytes, example->length, &value) == example->expected);
    CHECK(value.worktree.modified == example->modified && value.worktree.deleted == example->deleted);
    CHECK(value.worktree.untracked == example->untracked && value.worktree.conflicted == example->conflicts);
    CHECK(value.index.staged_paths == example->staged && value.index.staged_gitlinks == example->gitlinks);
    CHECK(value.index.conflicted_paths == example->conflicts && value.submodule_dirty == example->submodules);
    if (example->expected != UMI_STATUS_OK) {
        const unsigned char *bytes = (const unsigned char *)&value;
        for (size_t i = 0U; i < sizeof value; ++i) CHECK(bytes[i] == 0U);
    }
    if (strcmp(argv[1], "tracking") == 0) {
        CHECK(strcmp(value.branch.branch, "main") == 0 && strcmp(value.branch.upstream, "origin/main") == 0);
        CHECK(value.branch.ahead == 2U && value.branch.behind == 3U);
    }
    if (strcmp(argv[1], "detached") == 0) CHECK(value.branch.detached);
    return 0;
}
