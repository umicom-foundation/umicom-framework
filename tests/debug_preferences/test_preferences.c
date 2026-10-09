/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_preferences/test_preferences.c
 * PURPOSE: Check strict native-adapter preferences and durable Unicode file round trips without launching adapters.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "umicom/debug_runtime/adapter_preferences.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    UmiDebugAdapterPreferences preferences = {0}, decoded = {0}, before;
    strcpy(preferences.kind, "gdb");
#ifdef _WIN32
    const char *program = "C:/tools/caf\xc3\xa9/gdb.exe";
#else
    const char *program = "/tools/caf\xc3\xa9/gdb";
#endif
    strcpy(preferences.executable, program);
    char encoded[8192];
    size_t size = 0U;
    CHECK(UmiDebugAdapterPreferencesEncode(&preferences, encoded, sizeof(encoded), &size) == UMI_STATUS_OK);
    if (strcmp(mode, "round-trip") == 0 || strcmp(mode, "path-default") == 0)
    {
        if (strcmp(mode, "path-default") == 0)
        {
            preferences.executable[0] = '\0';
            CHECK(UmiDebugAdapterPreferencesEncode(&preferences, encoded, sizeof(encoded), &size) ==
                  UMI_STATUS_OK);
        }
        CHECK(UmiDebugAdapterPreferencesDecode(encoded, size, &decoded) == UMI_STATUS_OK);
/* The former byte comparison included stale bytes after cleared strings. Compare both persisted fields and retain the earlier assertion for review. */
#if 0
        CHECK(memcmp(&preferences, &decoded, sizeof(decoded)) == 0);
#endif
        /* An empty executable means PATH discovery. Bytes after its terminator
         * are not stored preferences and must not affect a successful round trip. */
        CHECK(strcmp(preferences.kind, decoded.kind) == 0 &&
              strcmp(preferences.executable, decoded.executable) == 0);
    }
    else if (strcmp(mode, "atomic-output") == 0)
    {
        strcpy(encoded, "retained");
        size = 99U;
        CHECK(UmiDebugAdapterPreferencesEncode(&preferences, encoded, 2U, &size) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(encoded, "retained") == 0 && size == 99U);
    }
    else if (strcmp(mode, "invalid-fields") == 0)
    {
        strcpy(preferences.kind, "unknown");
        CHECK(UmiDebugAdapterPreferencesValidate(&preferences) == UMI_STATUS_NOT_IMPLEMENTED);
        strcpy(preferences.kind, "gdb");
        strcpy(preferences.executable, "relative/gdb");
        CHECK(UmiDebugAdapterPreferencesValidate(&preferences) != UMI_STATUS_OK);
        memset(preferences.executable, 'x', sizeof(preferences.executable));
        CHECK(UmiDebugAdapterPreferencesValidate(&preferences) == UMI_STATUS_INVALID_ARGUMENT);
    }
    else if (strcmp(mode, "documents") == 0)
    {
        const char *bad[] = {
            "{}",
            "[]",
            "{\"format\":\"other\",\"adapter\":\"gdb\",\"executable\":\"\"}",
            "{\"format\":\"umicom.native-debug-adapter\",\"adapter\":\"gdb\",\"executable\":\"\",\"run\":"
            "true}",
            "{\"format\":\"umicom.native-debug-adapter\",\"adapter\":\"gdb\",\"ad\\u0061pter\":\"lldb\"}",
            "{\"format\":\"umicom.native-debug-adapter\",\"adapter\":\"gdb\",\"executable\":4}"};
        memset(&before, 0x5a, sizeof(before));
        for (size_t i = 0U; i < sizeof(bad) / sizeof(bad[0]); ++i)
        {
            decoded = before;
            CHECK(UmiDebugAdapterPreferencesDecode(bad[i], strlen(bad[i]), &decoded) != UMI_STATUS_OK);
            CHECK(memcmp(&decoded, &before, sizeof(decoded)) == 0);
        }
    }
    else if (strcmp(mode, "save-load") == 0 || strcmp(mode, "invalid-save") == 0)
    {
        char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
        FixtureDirectory(root);
        FixturePath(path, root, "caf\xc3\xa9.json");
        CHECK(UmiDebugAdapterPreferencesSave(path, &preferences) == UMI_STATUS_OK);
        before = preferences;
        if (strcmp(mode, "invalid-save") == 0)
        {
            strcpy(preferences.kind, "bad");
            CHECK(UmiDebugAdapterPreferencesSave(path, &preferences) != UMI_STATUS_OK);
        }
        CHECK(UmiDebugAdapterPreferencesLoad(path, &decoded) == UMI_STATUS_OK &&
              memcmp(&decoded, &before, sizeof(before)) == 0);
    }
    else if (strcmp(mode, "user-path") == 0)
    {
        char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
        FixtureDirectory(root);
        CHECK(UmiDebugAdapterPreferencesPath("Studio", root, path, sizeof(path)) == UMI_STATUS_OK);
        CHECK(umi_path_is_within(root, path));
        CHECK(strstr(path, "native-debug-adapter.json") != NULL);
        strcpy(path, "retained");
        CHECK(UmiDebugAdapterPreferencesPath("Studio", "relative", path, sizeof(path)) != UMI_STATUS_OK);
        CHECK(strcmp(path, "retained") == 0);
    }
    else
        return 2;
    return 0;
}
