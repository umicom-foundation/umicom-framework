/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_recovery_storage.c
 * PURPOSE: Check append-only recovery publication and bounded discovery without modifying a source file.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/document/recovery_storage.h"
#include "umicom/platform/rooted_files.h"
#include "umicom/platform/filesystem.h"
#ifndef _WIN32
#include <unistd.h>
#endif

#ifdef _WIN32
/* Exercise the same link rejection contracts on Windows whenever the host can
 * create symbolic links. Developer Mode permits the unprivileged form; older
 * systems may still allow the ordinary form through their process privilege.
 * Only a missing platform capability skips coverage; other errors fail it. */
static int RecoveryLink(const char *alias, const char *target, int directory)
{
    wchar_t alias_wide[UMI_PATH_CAPACITY], target_wide[UMI_PATH_CAPACITY];
    CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, alias, -1, alias_wide,
                              (int)UMI_PATH_CAPACITY) > 0);
    CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, target, -1, target_wide,
                              (int)UMI_PATH_CAPACITY) > 0);
    DWORD flags = directory ? SYMBOLIC_LINK_FLAG_DIRECTORY : 0U;
    /* The value is the Windows unprivileged-create flag, also accepted when
     * compiling against SDK headers that predate its symbolic definition. */
    if (CreateSymbolicLinkW(alias_wide, target_wide, flags | 2U))
        return 1;
    DWORD error = GetLastError();
    if (error == ERROR_INVALID_PARAMETER)
    {
        if (CreateSymbolicLinkW(alias_wide, target_wide, flags))
            return 1;
        error = GetLastError();
    }
    if (error == ERROR_PRIVILEGE_NOT_HELD || error == ERROR_NOT_SUPPORTED ||
        error == ERROR_INVALID_FUNCTION)
    {
        fprintf(stderr, "Symbolic-link fixture unavailable on this Windows host (error %lu).\n",
                (unsigned long)error);
        return 0;
    }
    fprintf(stderr, "Symbolic-link fixture creation failed (error %lu).\n", (unsigned long)error);
    CHECK(0);
    return 0;
}
#endif
static UmiDocumentRecoveryDraft *Make(const char *key, const char *text)
{
    UmiDocumentRecoveryInfo info = {0};
    strcpy(info.key, key);
    strcpy(info.display_name, "draft.c");
    strcpy(info.language_id, "c");
    strcpy(info.source_path, "/descriptive/only/source.c");
    info.text_bytes = strlen(text);
    UmiDocumentRecoveryDraft *draft = NULL;
    CHECK(UmiDocumentRecoveryDraftCreate(&info, text, strlen(text), NULL, &draft) == UMI_STATUS_OK);
    return draft;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1], *cases[] = {"save-load",           "exclusive",
                                            "collision-different", "missing-directory",
                                            "empty-list",          "list-limit",
                                            "scan-limit",          "ignore-names",
                                            "list-directory",      "file-root",
                                            "relative-root",       "traversal-root",
                                            "unicode-root",        "malformed",
                                            "truncated",           "mismatched-key",
                                            "oversized",           "cancelled-save",
                                            "cancelled-load",      "cancelled-list",
                                            "invalid-key",         "leaf-symlink",
                                            "root-symlink",        "directory-path",
                                            "directory-capacity",  "directory-relative-base",
                                            "null-save",           "null-load",
                                            "null-list",           "list-index"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = 1;
    CHECK(known);
    char root[UMI_PATH_CAPACITY], directory[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    strcpy(directory, root);
    const char *key = "0123456789abcdef0123456789abcdef", *other = "fedcba9876543210fedcba9876543210";
    char leaf[39];
    (void)snprintf(leaf, sizeof(leaf), "%s.draft", key);
    UmiDocumentRecoveryDraft *draft = Make(key, "unsaved\r\nsource"), *loaded = NULL;
    UmiDocumentRecoveryCatalogue *catalogue = NULL;
    if (strcmp(mode, "unicode-root") == 0)
    {
        FixturePath(directory, root, "caf\xc3\xa9");
        CHECK(umi_fs_make_directories(directory) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "directory-path") == 0 || strcmp(mode, "directory-capacity") == 0 ||
        strcmp(mode, "directory-relative-base") == 0)
    {
        UmiStatus status = UmiDocumentRecoveryDirectory(
            "RecoveryTest", strcmp(mode, "directory-relative-base") == 0 ? "relative" : root, path,
            strcmp(mode, "directory-capacity") == 0 ? 2U : sizeof(path));
        if (strcmp(mode, "directory-path") == 0)
            CHECK(status == UMI_STATUS_OK && umi_path_is_within(root, path));
        else
            CHECK(status != UMI_STATUS_OK);
        goto done;
    }
    if (strcmp(mode, "empty-list") == 0 || strcmp(mode, "missing-directory") == 0)
    {
        if (strcmp(mode, "missing-directory") == 0)
            FixturePath(directory, root, "absent");
        CHECK(UmiDocumentRecoveryList(directory, NULL, &catalogue) == UMI_STATUS_OK &&
              UmiDocumentRecoveryCatalogueCount(catalogue) == 0U);
        if (strcmp(mode, "missing-directory") == 0)
            CHECK(UmiDocumentRecoverySave(directory, draft, NULL) == UMI_STATUS_NOT_FOUND);
        goto done;
    }
    if (strcmp(mode, "relative-root") == 0 || strcmp(mode, "traversal-root") == 0)
    {
        if (strcmp(mode, "relative-root") == 0)
            strcpy(directory, "relative");
        else
            FixturePath(directory, root, "../outside");
        CHECK(UmiDocumentRecoverySave(directory, draft, NULL) != UMI_STATUS_OK);
        CHECK(UmiDocumentRecoveryList(directory, NULL, &catalogue) != UMI_STATUS_OK && catalogue == NULL);
        goto done;
    }
    if (strcmp(mode, "cancelled-save") == 0 || strcmp(mode, "cancelled-list") == 0)
    {
        UmiCancellationToken *cancel = NULL;
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        umi_cancellation_token_request(cancel);
        UmiStatus status = strcmp(mode, "cancelled-save") == 0
                               ? UmiDocumentRecoverySave(directory, draft, cancel)
                               : UmiDocumentRecoveryList(directory, cancel, &catalogue);
        CHECK(status == UMI_STATUS_CANCELLED);
        umi_cancellation_token_destroy(cancel);
        CHECK(UmiDocumentRecoveryList(directory, NULL, &catalogue) == UMI_STATUS_OK &&
              UmiDocumentRecoveryCatalogueCount(catalogue) == 0U);
        goto done;
    }
    if (strcmp(mode, "null-save") == 0)
    {
        CHECK(UmiDocumentRecoverySave(directory, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        goto done;
    }
    if (strcmp(mode, "null-load") == 0)
    {
        CHECK(UmiDocumentRecoveryLoad(directory, key, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        goto done;
    }
    if (strcmp(mode, "null-list") == 0)
    {
        CHECK(UmiDocumentRecoveryList(directory, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        goto done;
    }
    if (strcmp(mode, "file-root") == 0)
    {
        FixturePath(directory, root, "ordinary");
        CHECK(UmiRootedFileWrite(root, "ordinary", "x", 1U) == UMI_STATUS_OK);
        CHECK(UmiDocumentRecoverySave(directory, draft, NULL) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(UmiDocumentRecoveryList(directory, NULL, &catalogue) == UMI_STATUS_PERMISSION_DENIED &&
              catalogue == NULL);
        goto done;
    }
    if (strcmp(mode, "root-symlink") == 0 || strcmp(mode, "leaf-symlink") == 0)
    {
#ifdef _WIN32
/* Windows link fixtures now probe the actual host capability instead of skipping unconditionally. The former implementation is retained for review. */
#if 0
        UmiDocumentRecoveryDraftDestroy(draft);
        return 77;
#endif
        if (strcmp(mode, "root-symlink") == 0)
        {
            FixturePath(directory, root, "alias");
            if (!RecoveryLink(directory, root, 1))
            {
                UmiDocumentRecoveryDraftDestroy(draft);
                return 77;
            }
            CHECK(UmiDocumentRecoverySave(directory, draft, NULL) == UMI_STATUS_PERMISSION_DENIED);
            CHECK(UmiDocumentRecoveryList(directory, NULL, &catalogue) == UMI_STATUS_PERMISSION_DENIED);
        }
        else
        {
            FixturePath(path, root, leaf);
            if (!RecoveryLink(path, "missing", 0))
            {
                UmiDocumentRecoveryDraftDestroy(draft);
                return 77;
            }
            CHECK(UmiDocumentRecoverySave(root, draft, NULL) == UMI_STATUS_ALREADY_EXISTS);
            CHECK(UmiDocumentRecoveryLoad(root, key, NULL, &loaded) != UMI_STATUS_OK && loaded == NULL);
            CHECK(UmiDocumentRecoveryList(root, NULL, &catalogue) == UMI_STATUS_OK);
            UmiDocumentRecoveryEntry entry;
            CHECK(UmiDocumentRecoveryCatalogueAt(catalogue, 0U, &entry) == UMI_STATUS_OK &&
                  entry.availability == UMI_STATUS_PERMISSION_DENIED);
        }
        goto done;
#else
        if (strcmp(mode, "root-symlink") == 0)
        {
            FixturePath(directory, root, "alias");
            CHECK(symlink(root, directory) == 0);
            CHECK(UmiDocumentRecoverySave(directory, draft, NULL) == UMI_STATUS_PERMISSION_DENIED);
            CHECK(UmiDocumentRecoveryList(directory, NULL, &catalogue) == UMI_STATUS_PERMISSION_DENIED);
        }
        else
        {
            FixturePath(path, root, leaf);
            CHECK(symlink("missing", path) == 0);
            CHECK(UmiDocumentRecoverySave(root, draft, NULL) == UMI_STATUS_ALREADY_EXISTS);
            CHECK(UmiDocumentRecoveryLoad(root, key, NULL, &loaded) != UMI_STATUS_OK && loaded == NULL);
            CHECK(UmiDocumentRecoveryList(root, NULL, &catalogue) == UMI_STATUS_OK);
            UmiDocumentRecoveryEntry entry;
            CHECK(UmiDocumentRecoveryCatalogueAt(catalogue, 0U, &entry) == UMI_STATUS_OK &&
                  entry.availability == UMI_STATUS_PERMISSION_DENIED);
        }
        goto done;
#endif
    }
    CHECK(UmiDocumentRecoverySave(directory, draft, NULL) == UMI_STATUS_OK);
    if (strcmp(mode, "exclusive") == 0 || strcmp(mode, "collision-different") == 0)
    {
        UmiDocumentRecoveryDraft *collision =
            strcmp(mode, "collision-different") == 0 ? Make(key, "different") : draft;
        CHECK(UmiDocumentRecoverySave(directory, collision, NULL) == UMI_STATUS_ALREADY_EXISTS);
        if (collision != draft)
            UmiDocumentRecoveryDraftDestroy(collision);
    }
    if (strcmp(mode, "invalid-key") == 0)
    {
        CHECK(UmiDocumentRecoveryLoad(directory, "../source", NULL, &loaded) == UMI_STATUS_INVALID_ARGUMENT &&
              loaded == NULL);
        goto done;
    }
    if (strcmp(mode, "cancelled-load") == 0)
    {
        UmiCancellationToken *cancel = NULL;
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        umi_cancellation_token_request(cancel);
        CHECK(UmiDocumentRecoveryLoad(directory, key, cancel, &loaded) == UMI_STATUS_CANCELLED &&
              loaded == NULL);
        umi_cancellation_token_destroy(cancel);
        goto done;
    }
    if (strcmp(mode, "malformed") == 0 || strcmp(mode, "truncated") == 0 ||
        strcmp(mode, "mismatched-key") == 0)
    {
        unsigned char *encoded = NULL;
        size_t bytes = 0U;
        UmiDocumentRecoveryDraft *wrong = Make(other, "other");
        CHECK(UmiDocumentRecoveryDraftEncode(strcmp(mode, "mismatched-key") == 0 ? wrong : draft, &encoded,
                                             &bytes) == UMI_STATUS_OK);
        if (strcmp(mode, "malformed") == 0)
            encoded[0] = 'X';
        if (strcmp(mode, "truncated") == 0)
            --bytes;
        CHECK(UmiRootedFileWrite(directory, leaf, encoded, bytes) == UMI_STATUS_OK);
        CHECK(UmiDocumentRecoveryLoad(directory, key, NULL, &loaded) != UMI_STATUS_OK && loaded == NULL);
        UmiDocumentRecoveryBytesFree(encoded);
        UmiDocumentRecoveryDraftDestroy(wrong);
        goto done;
    }
    if (strcmp(mode, "oversized") == 0)
    {
        size_t size = UMI_DOCUMENT_RECOVERY_RECORD_LIMIT + 1U;
        char *large = calloc(size, 1U);
        CHECK(large);
        CHECK(UmiRootedFileWrite(directory, leaf, large, size) == UMI_STATUS_OK);
        free(large);
        CHECK(UmiDocumentRecoveryLoad(directory, key, NULL, &loaded) == UMI_STATUS_CAPACITY_EXCEEDED &&
              loaded == NULL);
        CHECK(UmiDocumentRecoveryList(directory, NULL, &catalogue) == UMI_STATUS_OK);
        UmiDocumentRecoveryEntry entry;
        CHECK(UmiDocumentRecoveryCatalogueAt(catalogue, 0U, &entry) == UMI_STATUS_OK &&
              entry.availability == UMI_STATUS_CAPACITY_EXCEEDED);
        goto done;
    }
    if (strcmp(mode, "list-limit") == 0 || strcmp(mode, "scan-limit") == 0)
    {
        size_t count = strcmp(mode, "list-limit") == 0 ? 256U : 4096U;
        for (size_t i = 0U; i < count; ++i)
        {
            char name[80];
            if (strcmp(mode, "list-limit") == 0)
                (void)snprintf(name, sizeof(name), "%032zu.draft", i);
            else
                (void)snprintf(name, sizeof(name), "unrelated-%zu", i);
            CHECK(UmiRootedFileWrite(directory, name, "x", 1U) == UMI_STATUS_OK);
        }
        CHECK(UmiDocumentRecoveryList(directory, NULL, &catalogue) == UMI_STATUS_CAPACITY_EXCEEDED &&
              catalogue == NULL);
        goto done;
    }
    if (strcmp(mode, "ignore-names") == 0)
    {
/* The old mixed-case name aliases the saved key on Windows and overwrites its contents. Keep that setup for review; the new name still checks rejection of uppercase keys without colliding. */
#if 0
        const char *names[] = {"source.c", "legacy.recovery", "../escape",
                               "0123456789ABCDEF0123456789abcdef.draft", "short.draft"};
#endif
        /* Use an invalid mixed-case key that is distinct even on case-insensitive
         * filesystems. The unrelated-name probe must not replace the valid draft. */
        const char *names[] = {"source.c", "legacy.recovery", "../escape",
                               "1123456789ABCDEF0123456789abcdef.draft", "short.draft"};
        for (size_t i = 0U; i < sizeof(names) / sizeof(names[0]); ++i)
            if (i != 2U)
                CHECK(UmiRootedFileWrite(directory, names[i], "x", 1U) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "list-directory") == 0)
    {
        char name[39];
        (void)snprintf(name, sizeof(name), "%s.draft", other);
        FixturePath(path, directory, name);
        CHECK(umi_fs_make_directories(path) == UMI_STATUS_OK);
    }
    CHECK(UmiDocumentRecoveryLoad(directory, key, NULL, &loaded) == UMI_STATUS_OK);
    const char *text = NULL;
    size_t bytes = 0U;
    CHECK(UmiDocumentRecoveryDraftRead(loaded, &text, &bytes) == UMI_STATUS_OK &&
          strcmp(text, "unsaved\r\nsource") == 0);
    CHECK(UmiDocumentRecoveryList(directory, NULL, &catalogue) == UMI_STATUS_OK);
    CHECK(UmiDocumentRecoveryCatalogueCount(catalogue) == (strcmp(mode, "list-directory") == 0 ? 2U : 1U));
    if (strcmp(mode, "list-index") == 0)
    {
        UmiDocumentRecoveryEntry entry;
        memset(&entry, 0x5a, sizeof(entry));
        UmiDocumentRecoveryEntry before = entry;
        CHECK(UmiDocumentRecoveryCatalogueAt(catalogue, 99U, &entry) == UMI_STATUS_NOT_FOUND &&
              memcmp(&entry, &before, sizeof(entry)) == 0);
    }
done:
    UmiDocumentRecoveryDraftDestroy(loaded);
    UmiDocumentRecoveryDraftDestroy(draft);
    UmiDocumentRecoveryCatalogueDestroy(catalogue);
    return 0;
}
