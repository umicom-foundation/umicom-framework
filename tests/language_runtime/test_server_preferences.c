/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_server_preferences.c
 * PURPOSE: Exercise local language-server preferences without starting a process.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "umicom/language_runtime/server_preferences.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    UmiLanguageServerPreferences preferences = {0}, decoded = {0}, sentinel;
    strcpy(preferences.language_id, "c");
#ifdef _WIN32
    strcpy(preferences.executable, "C:/tools/caf\xc3\xa9/clangd.exe");
#else
    strcpy(preferences.executable, "/tools/caf\xc3\xa9/clangd");
#endif
    strcpy(preferences.arguments, "--background-index '--query-driver=/tools/c compiler' \"\"");
    char encoded[32768];
    size_t size = 0U;
    memset(&sentinel, 0x5a, sizeof(sentinel));
    decoded = sentinel;
    CHECK(UmiLanguageServerPreferencesEncode(&preferences, encoded, sizeof(encoded), &size) == UMI_STATUS_OK);
    if (strcmp(mode, "round-trip") == 0 || strcmp(mode, "unicode") == 0 ||
        strcmp(mode, "empty-arguments") == 0)
    {
        if (strcmp(mode, "unicode") == 0)
            strcpy(preferences.language_id, "caf\xc3\xa9");
        if (strcmp(mode, "empty-arguments") == 0)
            /* Clear the complete value so the round-trip assertion compares
             * initialized storage as well as the empty argument spelling. */
            memset(preferences.arguments, 0, sizeof(preferences.arguments));
        CHECK(UmiLanguageServerPreferencesEncode(&preferences, encoded, sizeof(encoded), &size) ==
              UMI_STATUS_OK);
        CHECK(UmiLanguageServerPreferencesDecode(encoded, size, &decoded) == UMI_STATUS_OK);
        CHECK(memcmp(&preferences, &decoded, sizeof(decoded)) == 0);
    }
    else if (strcmp(mode, "capacity") == 0)
    {
        strcpy(encoded, "retained");
        size = 99U;
        CHECK(UmiLanguageServerPreferencesEncode(&preferences, encoded, 3U, &size) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(encoded, "retained") == 0 && size == 99U);
    }
    else if (strcmp(mode, "unterminated") == 0 || strcmp(mode, "relative") == 0 ||
             strcmp(mode, "invalid-unicode") == 0 || strcmp(mode, "control") == 0 ||
             strcmp(mode, "arguments") == 0 || strcmp(mode, "empty-language") == 0)
    {
        if (strcmp(mode, "unterminated") == 0)
            memset(preferences.executable, 'x', sizeof(preferences.executable));
        if (strcmp(mode, "relative") == 0)
            strcpy(preferences.executable, "clangd");
        if (strcmp(mode, "invalid-unicode") == 0)
            strcpy(preferences.language_id, "\xc0\x80");
        if (strcmp(mode, "control") == 0)
            strcpy(preferences.arguments, "--log\nverbose");
        if (strcmp(mode, "arguments") == 0)
            strcpy(preferences.arguments, "'unclosed");
        if (strcmp(mode, "empty-language") == 0)
            preferences.language_id[0] = '\0';
        CHECK(UmiLanguageServerPreferencesValidate(&preferences) != UMI_STATUS_OK);
    }
    else if (strcmp(mode, "documents") == 0)
    {
        const char *invalid[] = {
            "{}",
            "[]",
            "{\"format\":\"unknown\",\"language\":\"c\",\"executable\":\"/clangd\",\"arguments\":\"\"}",
            "{\"format\":\"umicom.local-language-server\",\"language\":\"c\",\"executable\":\"/"
            "clangd\",\"arguments\":\"\",\"run\":true}",
            "{\"format\":\"umicom.local-language-server\",\"language\":\"c\",\"langu\\u0061ge\":\"c\","
            "\"arguments\":\"\"}",
            "{\"format\":\"umicom.local-language-server\",\"language\":\"c\",\"executable\":\"/"
            "clangd\",\"arguments\":7}",
            "{\"format\":\"umicom.local-language-server\",\"language\":\"c\\u0000xx\",\"executable\":\"/"
            "clangd\",\"arguments\":\"\"}"};
        for (size_t i = 0U; i < sizeof(invalid) / sizeof(invalid[0]); ++i)
        {
            CHECK(UmiLanguageServerPreferencesDecode(invalid[i], strlen(invalid[i]), &decoded) !=
                  UMI_STATUS_OK);
            CHECK(memcmp(&decoded, &sentinel, sizeof(decoded)) == 0);
        }
    }
    else if (strcmp(mode, "byte-limit") == 0)
    {
        char *large = malloc(32769U);
        CHECK(large != NULL);
        memset(large, ' ', 32769U);
        CHECK(UmiLanguageServerPreferencesDecode(large, 32769U, &decoded) != UMI_STATUS_OK);
        CHECK(memcmp(&decoded, &sentinel, sizeof(decoded)) == 0);
        free(large);
    }
    else if (strcmp(mode, "save-load") == 0 || strcmp(mode, "invalid-save") == 0 ||
             strcmp(mode, "language-mismatch") == 0 || strcmp(mode, "missing") == 0)
    {
        char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
        FixtureDirectory(root);
        FixturePath(path, root, "caf\xc3\xa9.json");
        if (strcmp(mode, "missing") == 0)
        {
            CHECK(UmiLanguageServerPreferencesLoad(path, "c", &decoded) != UMI_STATUS_OK);
            CHECK(memcmp(&decoded, &sentinel, sizeof(decoded)) == 0);
        }
        else
        {
            CHECK(UmiLanguageServerPreferencesSave(path, &preferences) == UMI_STATUS_OK);
            if (strcmp(mode, "invalid-save") == 0)
            {
                UmiLanguageServerPreferences invalid = preferences;
                strcpy(invalid.executable, "relative");
                CHECK(UmiLanguageServerPreferencesSave(path, &invalid) != UMI_STATUS_OK);
            }
            if (strcmp(mode, "language-mismatch") == 0)
            {
                CHECK(UmiLanguageServerPreferencesLoad(path, "C", &decoded) == UMI_STATUS_INVALID_STATE);
                CHECK(memcmp(&decoded, &sentinel, sizeof(decoded)) == 0);
            }
            else
            {
                CHECK(UmiLanguageServerPreferencesLoad(path, "c", &decoded) == UMI_STATUS_OK);
                CHECK(memcmp(&decoded, &preferences, sizeof(decoded)) == 0);
            }
        }
    }
    else if (strcmp(mode, "user-path") == 0 || strcmp(mode, "case-sensitive") == 0 ||
             strcmp(mode, "path-separator") == 0 || strcmp(mode, "path-capacity") == 0 ||
             strcmp(mode, "invalid-base") == 0 || strcmp(mode, "language-limit") == 0)
    {
        char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY], other[UMI_PATH_CAPACITY];
        FixtureDirectory(root);
        CHECK(UmiLanguageServerPreferencesPath("Studio", root, "c", path, sizeof(path)) == UMI_STATUS_OK);
        CHECK(umi_path_is_within(root, path) && strstr(path, "language-server-63.json") != NULL);
        if (strcmp(mode, "case-sensitive") == 0)
        {
            CHECK(UmiLanguageServerPreferencesPath("Studio", root, "C", other, sizeof(other)) ==
                  UMI_STATUS_OK);
            CHECK(strstr(other, "language-server-43.json") != NULL && strcmp(path, other) != 0);
        }
        else if (strcmp(mode, "path-separator") == 0)
        {
            CHECK(UmiLanguageServerPreferencesPath("Studio", root, "../c", other, sizeof(other)) ==
                  UMI_STATUS_OK);
            CHECK(umi_path_is_within(root, other) && strstr(other, "language-server-2e2e2f63.json") != NULL);
        }
        else if (strcmp(mode, "path-capacity") == 0 || strcmp(mode, "invalid-base") == 0)
        {
            strcpy(other, "retained");
            CHECK(UmiLanguageServerPreferencesPath(
                      "Studio", strcmp(mode, "invalid-base") == 0 ? "relative" : root, "c", other,
                      strcmp(mode, "path-capacity") == 0 ? 3U : sizeof(other)) != UMI_STATUS_OK);
            CHECK(strcmp(other, "retained") == 0);
        }
        else if (strcmp(mode, "language-limit") == 0)
        {
            char language[65];
            memset(language, 'x', 64U);
            language[64] = '\0';
            strcpy(other, "retained");
            CHECK(UmiLanguageServerPreferencesPath("Studio", root, language, other, sizeof(other)) !=
                  UMI_STATUS_OK);
            CHECK(strcmp(other, "retained") == 0);
            language[63] = '\0';
            CHECK(UmiLanguageServerPreferencesPath("Studio", root, language, other, sizeof(other)) ==
                  UMI_STATUS_OK);
        }
    }
    else
        return 2;
    return 0;
}
