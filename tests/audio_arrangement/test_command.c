/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/audio_arrangement/test_command.c
 * PURPOSE: Exercise command dispatch against isolated UTF-8 paths and preserve existing destinations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#undef CHECK
#include "fixture.h"
#include "../../examples/audio_arrangement/command.h"
#include "umicom/platform/rooted_files.h"
#include "umicom/platform/filesystem.h"
int main(int argc, char **argv)
{
    const char *cases[] = {"help",           "inspect",         "render",
                           "unicode",        "existing",        "missing-source",
                           "invalid-recipe", "invalid-command", "relative-output"};
    if (argc != 2 || !Known(argv[1], cases, sizeof(cases) / sizeof(cases[0])))
        return 2;
    const char *mode = argv[1];
    if (strcmp(mode, "help") == 0)
    {
        char *arguments[] = {"umicom-audio-arrangement", "--help"};
        CHECK(UmiAudioArrangementCommand(2, arguments) == 0);
        return 0;
    }
    if (strcmp(mode, "invalid-command") == 0)
    {
        char *arguments[] = {"umicom-audio-arrangement", "render"};
        CHECK(UmiAudioArrangementCommand(2, arguments) == 2);
        return 0;
    }
    char root[UMI_PATH_CAPACITY], library_path[UMI_PATH_CAPACITY], plan_path[UMI_PATH_CAPACITY],
        output[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(library_path, root, "source collection.umilibrary");
    FixturePath(plan_path, root, "mix plan.json");
    const char *leaf = strcmp(mode, "unicode") == 0 ? "caf\xc3\xa9 mix.wav" : "mix.wav";
    FixturePath(output, root, leaf);
    UmiCreativeAssetLibrary *library = Sources(8000U, 1U, 16000, 16000);
    UmiCreativeAssetWriteResult written;
    CHECK(UmiCreativeAssetLibrarySaveNew(library, library_path, NULL, &written) == UMI_STATUS_OK);
    UmiCreativeAssetLibraryDestroy(library);
    UmiCreativeAudioArrangement plan = Plan();
    if (strcmp(mode, "missing-source") == 0)
        strcpy(plan.clips[0].asset_id, "missing");
    UmiCreativeAsset *document = NULL;
    CHECK(UmiCreativeAudioArrangementExport(&plan, NULL, &document) == UMI_STATUS_OK);
    CHECK(UmiCreativeAssetWriteNew(document, plan_path, NULL, &written) == UMI_STATUS_OK);
    UmiCreativeAssetDestroy(document);
    if (strcmp(mode, "invalid-recipe") == 0)
        CHECK(UmiRootedFileWrite(root, "mix plan.json", "invalid", 7U) == UMI_STATUS_OK);
    if (strcmp(mode, "existing") == 0)
        CHECK(UmiRootedFileWrite(root, leaf, "keep", 4U) == UMI_STATUS_OK);
    if (strcmp(mode, "inspect") == 0)
    {
        char *arguments[] = {"umicom-audio-arrangement", "inspect", plan_path};
        CHECK(UmiAudioArrangementCommand(3, arguments) == 0);
        CHECK(!umi_fs_exists(output));
        return 0;
    }
    char *arguments[] = {"umicom-audio-arrangement", "render", library_path, plan_path,
                         strcmp(mode, "relative-output") == 0 ? "relative.wav" : output};
    int result = UmiAudioArrangementCommand(5, arguments);
    if (strcmp(mode, "existing") == 0)
    {
        CHECK(result == 1);
        size_t size = 0U;
        unsigned char *bytes = FixtureRead(output, &size);
        CHECK(size == 4U && memcmp(bytes, "keep", 4U) == 0);
        free(bytes);
    }
    else if (strcmp(mode, "missing-source") == 0 || strcmp(mode, "invalid-recipe") == 0 ||
             strcmp(mode, "relative-output") == 0)
        CHECK(result == 1 && !umi_fs_exists(output));
    else
    {
        CHECK(result == 0);
        size_t size = 0U;
        unsigned char *bytes = FixtureRead(output, &size);
        CHECK(size == 556U && memcmp(bytes, "RIFF", 4U) == 0 && bytes[44] == 128U && bytes[45] == 62U);
        free(bytes);
    }
    return 0;
}
