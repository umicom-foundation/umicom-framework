/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_revision_changes/test_parse.c
 * Purpose: Exercise binary Git revision records, immutable ownership and refusal boundaries.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/revision_changes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
typedef struct Packet { char *bytes; size_t size, capacity; } Packet;
static bool Record(Packet *packet, const char *oldMode, const char *newMode,
    const char *state, const char *path, size_t hashLength)
{
    char oldHash[65], newHash[65], header[192];
    memset(oldHash, strcmp(oldMode, "000000") == 0 ? '0' : '1', hashLength);
    memset(newHash, strcmp(newMode, "000000") == 0 ? '0' : '2', hashLength);
    oldHash[hashLength] = newHash[hashLength] = '\0';
    int n = snprintf(header, sizeof(header), ":%s %s %s %s %s", oldMode, newMode, oldHash, newHash, state);
    if (n < 0 || (size_t)n >= sizeof(header)) return false;
    size_t pathSize = strlen(path), needed = packet->size + (size_t)n + pathSize + 2U;
    if (needed > packet->capacity) {
        size_t capacity = needed * 2U;
        char *next = realloc(packet->bytes, capacity);
        if (next == NULL) return false;
        packet->bytes = next; packet->capacity = capacity;
    }
    memcpy(packet->bytes + packet->size, header, (size_t)n + 1U);
    packet->size += (size_t)n + 1U;
    memcpy(packet->bytes + packet->size, path, pathSize + 1U);
    packet->size += pathSize + 1U;
    return true;
}
static void Clear(Packet *packet)
{
    free(packet->bytes); memset(packet, 0, sizeof(*packet));
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    Packet p = {0};
    UmiVcsRevisionChanges *changes = NULL;
    if (strcmp(mode, "empty") == 0) {
        CHECK(UmiVcsRevisionChangesParse(NULL, 0U, &changes) == UMI_STATUS_OK);
        CHECK(UmiVcsRevisionChangesCount(changes) == 0U);
    } else if (strcmp(mode, "malformed") == 0) {
        const char *states[] = {"R100", "C100", "U", "X", "M42", "", "MM"};
        for (size_t i = 0U; i < sizeof(states) / sizeof(states[0]); ++i) {
            CHECK(Record(&p, "100644", "100644", states[i], "a.c", 40U));
            CHECK(UmiVcsRevisionChangesParse(p.bytes, p.size, &changes) == UMI_STATUS_PARSE_ERROR && changes == NULL);
            Clear(&p);
        }
        CHECK(Record(&p, "100644", "100644", "A", "a.c", 40U));
        CHECK(UmiVcsRevisionChangesParse(p.bytes, p.size, &changes) == UMI_STATUS_PARSE_ERROR);
        Clear(&p);
        CHECK(Record(&p, "999999", "100644", "M", "a.c", 40U));
        CHECK(UmiVcsRevisionChangesParse(p.bytes, p.size, &changes) == UMI_STATUS_PARSE_ERROR);
        Clear(&p);
        CHECK(Record(&p, "100644", "100644", "M", "a.c", 40U));
        p.bytes[15] = 'g';
        CHECK(UmiVcsRevisionChangesParse(p.bytes, p.size, &changes) == UMI_STATUS_PARSE_ERROR && changes == NULL);
    } else if (strcmp(mode, "truncated") == 0) {
        CHECK(Record(&p, "100644", "100644", "M", "a.c", 40U));
        for (size_t i = 1U; i < p.size; ++i)
            CHECK(UmiVcsRevisionChangesParse(p.bytes, i, &changes) == UMI_STATUS_PARSE_ERROR && changes == NULL);
    } else if (strcmp(mode, "duplicate") == 0) {
        CHECK(Record(&p, "100644", "100644", "M", "a.c", 40U));
        CHECK(Record(&p, "100644", "100644", "M", "a.c", 40U));
        CHECK(UmiVcsRevisionChangesParse(p.bytes, p.size, &changes) == UMI_STATUS_PARSE_ERROR && changes == NULL);
    } else if (strcmp(mode, "bad-path") == 0) {
        const char *paths[] = {"", "/outside.c", "../a.c", "src/../a.c", "./a.c", "src//a.c", "src/", "C:/a.c"};
        for (size_t i = 0U; i < sizeof(paths) / sizeof(paths[0]); ++i) {
            CHECK(Record(&p, "100644", "100644", "M", paths[i], 40U));
            CHECK(UmiVcsRevisionChangesParse(p.bytes, p.size, &changes) == UMI_STATUS_PARSE_ERROR && changes == NULL);
            Clear(&p);
        }
    } else if (strcmp(mode, "capacity") == 0) {
        char *longPath = malloc(UMI_VCS_REVISION_PATH_LIMIT + 2U);
        CHECK(longPath != NULL);
        memset(longPath, 'a', UMI_VCS_REVISION_PATH_LIMIT + 1U);
        longPath[UMI_VCS_REVISION_PATH_LIMIT + 1U] = '\0';
        CHECK(Record(&p, "100644", "100644", "M", longPath, 40U));
        free(longPath);
        CHECK(UmiVcsRevisionChangesParse(p.bytes, p.size, &changes) == UMI_STATUS_CAPACITY_EXCEEDED);
        Clear(&p);
        char byte = 0;
        CHECK(UmiVcsRevisionChangesParse(&byte, UMI_VCS_REVISION_CHANGE_BYTE_LIMIT + 1U, &changes) == UMI_STATUS_CAPACITY_EXCEEDED);
        for (size_t i = 0U; i <= UMI_VCS_REVISION_CHANGE_LIMIT; ++i)
            CHECK(Record(&p, "100644", "100644", "M", "a.c", 40U));
        CHECK(UmiVcsRevisionChangesParse(p.bytes, p.size, &changes) == UMI_STATUS_CAPACITY_EXCEEDED && changes == NULL);
    } else if (strcmp(mode, "invalid-request") == 0) {
        UmiVcsRevisionChangesRequest request = {0};
        CHECK(UmiVcsRevisionChangesRead(NULL, &changes) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiVcsRevisionChangesRead(&request, &changes) == UMI_STATUS_INVALID_ARGUMENT);
        request.repository_root = "unused";
        request.base_commit = "--output=outside";
        request.head_commit = "1111111111111111111111111111111111111111";
        CHECK(UmiVcsRevisionChangesRead(&request, &changes) == UMI_STATUS_INVALID_ARGUMENT);
        request.base_commit = "main";
        CHECK(UmiVcsRevisionChangesRead(&request, &changes) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(changes == NULL);
    } else {
        bool rename = strcmp(mode, "rename-pair") == 0;
        bool sorted = strcmp(mode, "basic-sort") == 0;
        bool gitlink = strcmp(mode, "gitlink") == 0;
        bool type = strcmp(mode, "type") == 0;
        bool sha256 = strcmp(mode, "sha256") == 0;
        bool ownership = strcmp(mode, "ownership") == 0;
        CHECK(rename || sorted || gitlink || type || sha256 || ownership);
        size_t hashLength = sha256 ? 64U : 40U;
        const char *name = ownership ? "tab\tand\nnewline.c" : "z.c";
        CHECK(Record(&p, gitlink ? "160000" : "100644", type ? "120000" : "100644", (type || gitlink) ? "T" : "M", name, hashLength));
        if (rename || sorted) {
            CHECK(Record(&p, "100644", "000000", "D", "old.c", hashLength));
            CHECK(Record(&p, "000000", "100644", "A", "a.c", hashLength));
        }
        CHECK(UmiVcsRevisionChangesParse(p.bytes, p.size, &changes) == UMI_STATUS_OK);
        CHECK(UmiVcsRevisionChangesCount(changes) == (rename || sorted ? 3U : 1U));
        const UmiVcsRevisionChange *first = UmiVcsRevisionChangesAt(changes, 0U);
        CHECK(first != NULL && strcmp(first->path, rename || sorted ? "a.c" : name) == 0);
        if (rename || sorted) {
            CHECK(first->status == 'A' && first->old_mode == 0U);
            CHECK(UmiVcsRevisionChangesAt(changes, 1U)->status == 'D');
        }
        if (gitlink) CHECK(first->gitlink && first->old_mode == 0160000U && first->new_mode == 0100644U);
        if (type) CHECK(!first->gitlink && first->new_mode == 0120000U);
        Clear(&p);
        CHECK(strcmp(first->path, rename || sorted ? "a.c" : name) == 0);
        UmiVcsRevisionChanges *saved = changes;
        CHECK(UmiVcsRevisionChangesParse(NULL, 0U, &changes) == UMI_STATUS_INVALID_ARGUMENT && changes == saved);
    }
    CHECK(UmiVcsRevisionChangesAt(changes, UmiVcsRevisionChangesCount(changes)) == NULL);
    UmiVcsRevisionChangesDestroy(changes); Clear(&p);
    return 0;
}
