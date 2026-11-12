/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform/test_remove_tree_posix.c
 *
 * PURPOSE:
 *   Exercise link, root, depth and native error boundaries in owned temporary trees.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
static const char *mode;
static int admission_only;
static struct dirent *FixtureRead(DIR *directory)
{
    if (!strcmp(mode, "read-error")) { errno = EIO; return NULL; }
    return readdir(directory);
}
static int FixtureClose(DIR *directory)
{
    int result = closedir(directory);
    if (!strcmp(mode, "close-error")) { errno = EIO; return -1; }
    return result;
}
static int FixtureUnlink(int parent, const char *name, int flags)
{
    if (admission_only || !strcmp(mode, "unlink-error")) { errno = EACCES; return -1; }
    return unlinkat(parent, name, flags);
}
static int FixtureOpen(int parent, const char *name, int flags)
{
    if (admission_only || !strcmp(mode, "open-error")) { errno = EACCES; return -1; }
    return openat(parent, name, flags);
}
/* The public Platform library supplies ordinary setup/cleanup. Only the
 * included remover's selected system boundary reports a deterministic error;
 * real temporary directory descriptors and entries are otherwise used. */
#define readdir FixtureRead
#define closedir FixtureClose
#define unlinkat FixtureUnlink
#define openat FixtureOpen
#include "../../src/platform/filesystem_remove_posix.inc"
#undef openat
#undef unlinkat
#undef closedir
#undef readdir
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s line %d: %s\n", mode, __LINE__, #x); failed = 1; goto finish; } } while (0)
static int Present(const char *path)
{
    struct stat item;
    return lstat(path, &item) == 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    mode = argv[1];
    const char *cases[] = {"null", "empty", "root", "root-slashes", "dot", "parent",
        "final-dot", "final-parent", "too-long", "missing", "file", "directory",
        "symlink-file", "symlink-directory", "dangling-link", "link-cycle",
        "trailing-link", "backslash-name", "depth", "depth-limit",
        "read-error", "close-error", "unlink-error", "open-error", "relative"};
    int known = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (!strcmp(mode, cases[i])) known = 1;
    if (!known) return 2;
    int failed = 0, owns = 0, changed_directory = 0;
    char root[] = "/tmp/umicom-remove-tree-XXXXXX";
    char target[UMI_PATH_CAPACITY] = {0}, outside[UMI_PATH_CAPACITY], keeper[UMI_PATH_CAPACITY];
    char input[UMI_PATH_CAPACITY], cwd[UMI_PATH_CAPACITY], deep[UMI_PATH_CAPACITY];
    size_t depth_created = 0U;
    char *readback = NULL;
    CHECK(mkdtemp(root) != NULL); owns = 1;
    CHECK(umi_path_join(root, "target", target, sizeof target) == UMI_STATUS_OK);
    CHECK(umi_path_join(root, "outside", outside, sizeof outside) == UMI_STATUS_OK);
    CHECK(umi_path_join(outside, "keep.txt", keeper, sizeof keeper) == UMI_STATUS_OK);
    CHECK(mkdir(outside, 0700) == 0);
    CHECK(umi_fs_write_text(keeper, "outside bytes remain") == UMI_STATUS_OK);
    const char *selected = target;
    UmiStatus expected = UMI_STATUS_OK;
    int is_error = strstr(mode, "-error") != NULL;
    if (!strcmp(mode, "null")) { selected = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(mode, "empty")) { selected = ""; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(mode, "root")) { selected = "/"; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(mode, "root-slashes")) { selected = "////"; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(mode, "dot")) { selected = "."; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(mode, "parent")) { selected = ".."; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(mode, "too-long")) {
        memset(input, 'x', sizeof input); input[sizeof input - 1U] = 'x';
        /* Exactly the bounded accessible span, intentionally no terminator. */
        selected = input; expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (!strcmp(mode, "final-dot") || !strcmp(mode, "final-parent")) {
        int n = snprintf(input, sizeof input, "%s/%s/", root,
            !strcmp(mode, "final-dot") ? "." : "..");
        CHECK(n > 0 && (size_t)n < sizeof input);
        selected = input; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (!strcmp(mode, "file")) {
        CHECK(umi_fs_write_text(target, "owned bytes") == UMI_STATUS_OK);
    } else if (!strcmp(mode, "symlink-file")) CHECK(symlink(keeper, target) == 0);
    else if (!strcmp(mode, "symlink-directory") || !strcmp(mode, "trailing-link")) {
        CHECK(symlink(outside, target) == 0);
        if (!strcmp(mode, "trailing-link")) {
            int n = snprintf(input, sizeof input, "%s///", target);
            CHECK(n > 0 && (size_t)n < sizeof input); selected = input;
        }
    } else if (!strcmp(mode, "dangling-link")) CHECK(symlink("never-created", target) == 0);
    else if (strcmp(mode, "missing")) {
        CHECK(mkdir(target, 0700) == 0);
        CHECK(umi_path_join(target, "entry", input, sizeof input) == UMI_STATUS_OK);
        if (!strcmp(mode, "link-cycle")) CHECK(symlink(".", input) == 0);
        else if (!strcmp(mode, "backslash-name")) {
            int n = snprintf(input, sizeof input, "%s/back\\slash", target);
            CHECK(n > 0 && (size_t)n < sizeof input);
            CHECK(umi_fs_write_text(input, "literal native filename") == UMI_STATUS_OK);
        } else if (!strcmp(mode, "depth") || !strcmp(mode, "depth-limit")) {
            strcpy(deep, target);
            size_t levels = !strcmp(mode, "depth") ? 127U : 128U;
            for (size_t i = 0U; i < levels; ++i) {
                size_t used = strlen(deep);
                CHECK(used + 2U < sizeof deep);
                memcpy(deep + used, "/d", 3U);
                CHECK(mkdir(deep, 0700) == 0); ++depth_created;
            }
            if (levels == 128U) expected = UMI_STATUS_CAPACITY_EXCEEDED;
        } else CHECK(umi_fs_write_text(input, "owned entry") == UMI_STATUS_OK);
        if (is_error) expected = UMI_STATUS_IO_ERROR;
        if (!strcmp(mode, "relative")) {
            CHECK(getcwd(cwd, sizeof cwd) != NULL);
            CHECK(chdir(root) == 0); changed_directory = 1; selected = "target";
        }
    }
    /* Refusal cases must not touch real ambient entries even if a future
     * admission regression accidentally reaches the native operation. */
    admission_only = expected == UMI_STATUS_INVALID_ARGUMENT ||
        (!strcmp(mode, "too-long"));
    CHECK(umi_posix_remove_tree(selected) == expected);
    if (expected == UMI_STATUS_OK) CHECK(!Present(target));
    else if (is_error || !strcmp(mode, "depth-limit")) CHECK(Present(target));
    CHECK(umi_fs_read_text(keeper, &readback, NULL) == UMI_STATUS_OK);
    CHECK(!strcmp(readback, "outside bytes remain"));
finish:
    umi_fs_free_text(readback);
    if (changed_directory && chdir(cwd) != 0) failed = 1;
    /* An over-depth fixture is unwound one known empty leaf at a time. Never
     * weaken the remover's depth contract merely to clean its regression data. */
    while (depth_created != 0U) {
        if (rmdir(deep) != 0 && errno != ENOENT) failed = 1;
        char *last = strrchr(deep, '/');
        if (last == NULL) { failed = 1; break; }
        *last = '\0'; --depth_created;
    }
    if (owns && umi_fs_remove_tree(root) != UMI_STATUS_OK) failed = 1;
    return failed;
}
#else
int main(void)
{
    puts("SKIP: POSIX descriptor and symbolic-link fixture; Windows reparse coverage is separate.");
    return 77;
}
#endif
