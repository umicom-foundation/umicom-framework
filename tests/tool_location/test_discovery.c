/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/tool_location/test_discovery.c
 * PURPOSE: Check supplied PATH ordering, absolute selection, capacity failures and native Unicode
 * snapshots. AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/process_search_path.h"
#include "umicom/toolchain/discovery.h"
#ifdef _WIN32
#define HOST_SEPARATOR ";"
#define TOOL_NAME "selection-probe.exe"
#else
#define HOST_SEPARATOR ":"
#define TOOL_NAME "selection-probe"
#endif
static int ProgramMain(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    char root[UMI_PATH_CAPACITY], first[UMI_PATH_CAPACITY], second[UMI_PATH_CAPACITY];
    char firstFile[UMI_PATH_CAPACITY], secondFile[UMI_PATH_CAPACITY], output[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(first, root, "tools with spaces");
    FixturePath(second, root, "tools-caf\xc3\xa9-\xd9\x85\xd9\x84\xd9\x81");
    CHECK(umi_fs_make_directories(first) == UMI_STATUS_OK);
    CHECK(umi_fs_make_directories(second) == UMI_STATUS_OK);
    FixturePath(firstFile, first, TOOL_NAME);
    FixturePath(secondFile, second, TOOL_NAME);
    /* These are deliberately data files: discovery must never execute them or
     * describe file existence as evidence of debugger/compiler compatibility. */
    CHECK(umi_fs_write_text(firstFile, "first installation\n") == UMI_STATUS_OK);
    CHECK(umi_fs_write_text(secondFile, "second installation\n") == UMI_STATUS_OK);
    strcpy(output, "unchanged");
    const char *mode = argv[1];
    if (strcmp(mode, "absolute") == 0)
    {
        CHECK(UmiToolchainFindInSearchPath(secondFile, NULL, output, sizeof output) ==
              UMI_STATUS_OK);
        CHECK(umi_path_equal(secondFile, output));
        CHECK(umi_toolchain_find_on_path(secondFile, output, sizeof output) == UMI_STATUS_OK);
        CHECK(umi_path_equal(secondFile, output));
    }
    else if (strcmp(mode, "order") == 0 || strcmp(mode, "unicode") == 0)
    {
        char search[UMI_PATH_CAPACITY * 2U + 4U];
        const char *head = strcmp(mode, "unicode") == 0 ? second : first;
        const char *tail = strcmp(mode, "unicode") == 0 ? first : second;
        int written = snprintf(search, sizeof search,
                               HOST_SEPARATOR "%s" HOST_SEPARATOR HOST_SEPARATOR "%s", head, tail);
        CHECK(written > 0 && (size_t)written < sizeof search);
        CHECK(UmiToolchainFindInSearchPath("selection-probe", search, output, sizeof output) ==
              UMI_STATUS_OK);
        CHECK(umi_path_equal(strcmp(mode, "unicode") == 0 ? secondFile : firstFile, output));
    }
    else if (strcmp(mode, "capacity") == 0)
    {
        CHECK(UmiToolchainFindInSearchPath("selection-probe", first, output, 2U) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(output, "unchanged") == 0);
        char *longPath = malloc(131073U);
        CHECK(longPath != NULL);
        memset(longPath, 'x', 131072U);
        longPath[131072U] = '\0';
        CHECK(UmiToolchainFindInSearchPath("selection-probe", longPath, output, sizeof output) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(output, "unchanged") == 0);
        free(longPath);
    }
    else if (strcmp(mode, "missing") == 0)
    {
        CHECK(UmiToolchainFindInSearchPath("selection-probe", "", output, sizeof output) ==
              UMI_STATUS_NOT_FOUND);
        CHECK(UmiToolchainFindInSearchPath("missing-probe", first, output, sizeof output) ==
              UMI_STATUS_NOT_FOUND);
        CHECK(UmiToolchainFindInSearchPath("", first, output, sizeof output) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(strcmp(output, "unchanged") == 0);
    }
    else if (strcmp(mode, "snapshot") == 0)
    {
        char *read = NULL, *captured = NULL, *joined = NULL;
        CHECK(UmiProcessSearchPathRead(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiProcessSearchPathRead(&read) == UMI_STATUS_OK && read != NULL);
        CHECK(UmiProcessSearchPathCapture(second, &captured) == UMI_STATUS_OK);
        CHECK(UmiProcessSearchPathJoin(second, read, &joined) == UMI_STATUS_OK);
        CHECK(strcmp(captured, joined) == 0);
        UmiProcessSearchPathFree(read);
        UmiProcessSearchPathFree(captured);
        UmiProcessSearchPathFree(joined);
    }
    else
        return 2;
    return 0;
}
#include "../native_process/utf8_entry.inc"
