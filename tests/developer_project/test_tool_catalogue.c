/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_tool_catalogue.c
 * PURPOSE: Check immutable tool-file selection without executing tool programs.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "umicom/developer_project/tool_catalogue.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/process_search_path.h"
#include "umicom/toolchain/discovery.h"

static void ToolFile(const char *folder, const char *name, char *out)
{
    CHECK(UmiProcessToolProgram(folder, name, out, UMI_PATH_CAPACITY) == UMI_STATUS_OK);
    /* Inert contents deliberately cannot act as a compiler. Inventory only
     * inspects file metadata; compatibility belongs to a later explicit run. */
    CHECK(umi_fs_write_text(out, "Inventory fixture; do not execute.\n") == UMI_STATUS_OK);
}
static UmiProjectToolFile Row(const UmiProjectToolCatalogue *catalogue, UmiProjectToolKind kind)
{
    UmiProjectToolFile row = {0};
    for (size_t index = 0U; index < UmiProjectToolCatalogueCount(catalogue); ++index)
    {
        CHECK(UmiProjectToolCatalogueAt(catalogue, index, &row) == UMI_STATUS_OK);
        if (row.kind == kind)
            return row;
    }
    CHECK(0 && "Requested tool row was absent");
    return row;
}
static int ProgramMain(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    char root[UMI_PATH_CAPACITY], tools[UMI_PATH_CAPACITY], other[UMI_PATH_CAPACITY];
    char compiler[UMI_PATH_CAPACITY], found[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(tools, root, "tools caf\xc3\xa9-\xd9\x85\xd9\x84\xd9\x81");
    FixturePath(other, root, "other installation");
    CHECK(umi_fs_make_directories(tools) == UMI_STATUS_OK);
    CHECK(umi_fs_make_directories(other) == UMI_STATUS_OK);
    UmiBuildProfile profile;
    CHECK(umi_build_profile_set(&profile, "catalogue", root, "build") == UMI_STATUS_OK);
    strcpy(profile.tool_directory, tools);
    const char *programs[] = {"cmake", "ctest", "cpack",    "ninja",
                              "cc",    "gdb",   "lldb-dap", "clangd"};
    if (strcmp(mode, "missing") != 0)
        for (size_t index = 0U; index < sizeof programs / sizeof programs[0]; ++index)
            ToolFile(tools, programs[index], found);
    strcpy(profile.compiler, "cc");
    UmiProjectToolCatalogue *catalogue = NULL;
    UmiCancellationToken *cancel = NULL;
    char *pathBefore = NULL, *pathAfter = NULL;
    CHECK(UmiProcessSearchPathRead(&pathBefore) == UMI_STATUS_OK);
    if (strcmp(mode, "invalid") == 0 || strcmp(mode, "cancel") == 0)
    {
        if (strcmp(mode, "invalid") == 0)
        {
            strcpy(profile.tool_directory, "relative/tools");
            CHECK(UmiProjectToolCatalogueRead(&profile, NULL, &catalogue) ==
                  UMI_STATUS_INVALID_ARGUMENT);
        }
        else
        {
            CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
            umi_cancellation_token_request(cancel);
            CHECK(UmiProjectToolCatalogueRead(&profile, cancel, &catalogue) ==
                  UMI_STATUS_CANCELLED);
        }
        CHECK(catalogue == NULL);
    }
    else
    {
        if (strcmp(mode, "automatic") == 0)
            profile.compiler[0] = '\0';
        else if (strcmp(mode, "absolute") == 0)
        {
            ToolFile(other, "cc", compiler);
            strcpy(profile.compiler, compiler);
        }
        else if (strcmp(mode, "invalid-compiler") == 0)
            strcpy(profile.compiler, "cc --malicious-argument");
        else if (strcmp(mode, "inherited") == 0)
            profile.tool_directory[0] = '\0';
        else if (strcmp(mode, "selected") != 0 && strcmp(mode, "missing") != 0 &&
                 strcmp(mode, "snapshot") != 0 && strcmp(mode, "boundaries") != 0)
            return 2;
        CHECK(UmiProjectToolCatalogueRead(&profile, NULL, &catalogue) == UMI_STATUS_OK);
        CHECK(UmiProjectToolCatalogueCount(catalogue) == 8U);
        UmiProjectToolFile selected = Row(catalogue, UMI_PROJECT_TOOL_COMPILER);
        if (strcmp(mode, "automatic") == 0)
        {
            CHECK(selected.selection == UMI_PROJECT_TOOL_AUTOMATIC);
            CHECK(selected.status == UMI_STATUS_NOT_IMPLEMENTED && selected.resolved[0] == '\0');
        }
        else if (strcmp(mode, "absolute") == 0)
        {
            CHECK(selected.selection == UMI_PROJECT_TOOL_EXPLICIT_FILE &&
                  selected.status == UMI_STATUS_OK);
            CHECK(umi_path_equal(selected.resolved, compiler));
        }
        else if (strcmp(mode, "invalid-compiler") == 0)
        {
            CHECK(selected.status == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(selected.selection == UMI_PROJECT_TOOL_FROM_DIRECTORY &&
                  selected.resolved[0] == '\0');
        }
        else if (strcmp(mode, "inherited") == 0)
        {
            /* Compare against the same PATH snapshot without assuming any tool
             * is installed on the machine running this regression case. */
            for (size_t index = 0U; index < UmiProjectToolCatalogueCount(catalogue); ++index)
            {
                UmiProjectToolFile file;
                CHECK(UmiProjectToolCatalogueAt(catalogue, index, &file) == UMI_STATUS_OK);
                UmiStatus expected =
                    UmiToolchainFindInSearchPath(file.requested, pathBefore, found, sizeof found);
                CHECK(file.selection == UMI_PROJECT_TOOL_FROM_PATH && file.status == expected);
                if (expected == UMI_STATUS_OK)
                    CHECK(umi_path_equal(file.resolved, found));
            }
        }
        else
        {
            for (size_t index = 0U; index < UmiProjectToolCatalogueCount(catalogue); ++index)
            {
                UmiProjectToolFile file;
                CHECK(UmiProjectToolCatalogueAt(catalogue, index, &file) == UMI_STATUS_OK);
                CHECK(file.selection == UMI_PROJECT_TOOL_FROM_DIRECTORY);
                CHECK(file.status ==
                      (strcmp(mode, "missing") == 0 ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK));
                if (file.status == UMI_STATUS_OK)
                {
                    CHECK(UmiProcessToolProgram(tools, file.requested, found, sizeof found) ==
                          UMI_STATUS_OK);
                    CHECK(umi_path_equal(file.resolved, found));
                }
                else
                    CHECK(file.resolved[0] == '\0');
            }
        }
        if (strcmp(mode, "snapshot") == 0)
        {
            /* Neither caller edits nor later filesystem changes mutate evidence
             * already captured. A separate read is required for fresh evidence. */
            strcpy(profile.tool_directory, other);
            selected.resolved[0] = '\0';
            selected = Row(catalogue, UMI_PROJECT_TOOL_COMPILER);
            CHECK(selected.resolved[0] != '\0');
            CHECK(umi_fs_remove_tree(selected.resolved) == UMI_STATUS_OK);
            UmiProjectToolFile stillCaptured = Row(catalogue, UMI_PROJECT_TOOL_COMPILER);
            CHECK(stillCaptured.status == UMI_STATUS_OK &&
                  strcmp(selected.resolved, stillCaptured.resolved) == 0);
            UmiProjectToolCatalogue *fresh = NULL;
            CHECK(UmiProjectToolCatalogueRead(&profile, NULL, &fresh) == UMI_STATUS_OK);
            UmiProjectToolFile changed = Row(fresh, UMI_PROJECT_TOOL_COMPILER);
            CHECK(changed.status == UMI_STATUS_NOT_FOUND);
            UmiProjectToolCatalogueDestroy(fresh);
        }
        if (strcmp(mode, "boundaries") == 0)
        {
            UmiProjectToolFile sentinel;
            memset(&sentinel, 0x4b, sizeof sentinel);
            UmiProjectToolFile original = sentinel;
            CHECK(UmiProjectToolCatalogueAt(catalogue, 8U, &sentinel) == UMI_STATUS_NOT_FOUND);
            CHECK(memcmp(&original, &sentinel, sizeof sentinel) == 0);
            CHECK(UmiProjectToolCatalogueAt(NULL, 0U, &sentinel) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(memcmp(&original, &sentinel, sizeof sentinel) == 0);
            CHECK(UmiProjectToolCatalogueAt(catalogue, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiProjectToolCatalogueRead(&profile, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiProjectToolCatalogueCount(NULL) == 0U);
            CHECK(strcmp(UmiProjectToolName((UmiProjectToolKind)99), "Unknown tool") == 0);
            UmiProjectToolCatalogueDestroy(NULL);
        }
    }
    CHECK(UmiProcessSearchPathRead(&pathAfter) == UMI_STATUS_OK);
    CHECK(strcmp(pathBefore, pathAfter) == 0);
    UmiProcessSearchPathFree(pathBefore);
    UmiProcessSearchPathFree(pathAfter);
    umi_cancellation_token_destroy(cancel);
    UmiProjectToolCatalogueDestroy(catalogue);
    return 0;
}
#include "../native_process/utf8_entry.inc"
