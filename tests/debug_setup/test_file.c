/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_setup/test_file.c
 * PURPOSE: Retain existing files and reject invalid setup files without mutating live state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/platform/path.h"
#include "umicom/platform/input_file.h"
#undef CHECK
#include "../build_log/fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(path, root, "setup-caf\xc3\xa9.json");
    UmiDebugSetup *setup = Setup(), *loaded = NULL;
    UmiOutputFileSnapshot receipt;
    if (strcmp(mode, "relative") == 0)
    {
        CHECK(UmiDebugSetupSaveNew("relative.json", setup, &receipt) != UMI_STATUS_OK &&
              receipt.path[0] == '\0');
    }
    else if (strcmp(mode, "missing") == 0)
    {
        CHECK(UmiDebugSetupLoad(path, &loaded) != UMI_STATUS_OK && loaded == NULL);
    }
    else if (strcmp(mode, "malformed") == 0)
    {
        UmiOutputFile *file = NULL;
        OK(UmiOutputFileCreate(path, &file));
        OK(UmiOutputFileWrite(file, "{bad", 4U));
        OK(UmiOutputFileClose(file));
        UmiOutputFileDestroy(file);
        CHECK(UmiDebugSetupLoad(path, &loaded) != UMI_STATUS_OK && loaded == NULL);
    }
    else if (strcmp(mode, "round-trip") == 0 || strcmp(mode, "no-overwrite") == 0)
    {
        OK(UmiDebugSetupSaveNew(path, setup, &receipt));
        CHECK(receipt.closed && receipt.bytes_written > 0U);
        if (strcmp(mode, "no-overwrite") == 0)
            CHECK(UmiDebugSetupSaveNew(path, setup, &receipt) != UMI_STATUS_OK);
        OK(UmiDebugSetupLoad(path, &loaded));
        char *a = NULL, *b = NULL;
        size_t sa = 0U, sb = 0U;
        OK(UmiDebugSetupEncode(setup, &a, &sa));
        OK(UmiDebugSetupEncode(loaded, &b, &sb));
        CHECK(sa == sb && memcmp(a, b, sa) == 0);
        UmiDebugSetupFreeBytes(a);
        UmiDebugSetupFreeBytes(b);
    }
    else
        return 2;
    UmiDebugSetupDestroy(loaded);
    UmiDebugSetupDestroy(setup);
    return 0;
}
