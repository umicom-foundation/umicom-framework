/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compilation_database/test_settings.c
 * PURPOSE: Verify explicit export edits and literal clangd arguments without launching tools.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/build/configure_definitions.h"
#include "umicom/language_runtime/arguments.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    if (strncmp(mode, "export-", 7U) == 0)
    {
        UmiBuildProfile *profile = malloc(sizeof *profile), *before = malloc(sizeof *before);
        CHECK(profile != NULL && before != NULL);
        umi_build_profile_init(profile);
        CHECK(umi_build_profile_set(profile, "review", DATABASE_ROOT, "build") == UMI_STATUS_OK);
        if (strcmp(mode, "export-existing") == 0)
            strcpy(profile->configure_definitions,
                   "-DFEATURE=one -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=OFF -DOTHER=\"two words\"");
        else if (strcmp(mode, "export-duplicate") == 0)
            strcpy(profile->configure_definitions,
                   "-DCMAKE_EXPORT_COMPILE_COMMANDS=OFF -DCMAKE_EXPORT_COMPILE_COMMANDS=ON");
        else if (strcmp(mode, "export-new") != 0)
            return 2;
        memcpy(before, profile, sizeof *profile);
        UmiStatus status = UmiCompilationDatabaseEnableExport(profile, profile);
        if (strcmp(mode, "export-duplicate") == 0)
        {
            CHECK(status != UMI_STATUS_OK);
            CHECK(memcmp(profile, before, sizeof *profile) == 0);
        }
        else
        {
            CHECK(status == UMI_STATUS_OK);
            UmiArguments *parsed = malloc(sizeof *parsed);
            CHECK(parsed != NULL);
            CHECK(UmiBuildConfigureDefinitions(profile, parsed) == UMI_STATUS_OK);
            if (strcmp(mode, "export-existing") == 0)
            {
                CHECK(parsed->count == 3U);
                CHECK(strcmp(parsed->values[0], "-DFEATURE=one") == 0);
                CHECK(strcmp(parsed->values[1], "-DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=ON") == 0);
                CHECK(strcmp(parsed->values[2], "-DOTHER=two words") == 0);
            }
            else
            {
                CHECK(parsed->count == 1U);
                CHECK(strcmp(parsed->values[0], "-DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=ON") == 0);
            }
            /* Only the explicit definition text is different. */
            strcpy(profile->configure_definitions, before->configure_definitions);
            CHECK(umi_build_profile_equal(profile, before));
            free(parsed);
        }
        free(before);
        free(profile);
        return EXIT_SUCCESS;
    }
    const char *input = "--background-index";
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "replace-equals") == 0)
        input = "--background-index --compile-commands-dir=/old";
    else if (strcmp(mode, "replace-separate") == 0)
        input = "--compile-commands-dir /old --background-index";
    else if (strcmp(mode, "single-dash") == 0)
        input = "-compile-commands-dir=/old --background-index";
    else if (strcmp(mode, "duplicate") == 0)
    {
        input = "--compile-commands-dir=/old -compile-commands-dir=/other";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(mode, "missing-value") == 0)
    {
        input = "--compile-commands-dir";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(mode, "response") == 0)
    {
        input = "@unread-response";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    }
    else if (strcmp(mode, "terminator") == 0)
    {
        input = "--";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    }
    else if (strcmp(mode, "small") == 0)
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    else if (strcmp(mode, "relative") == 0)
        expected = UMI_STATUS_INVALID_ARGUMENT;
    else if (strcmp(mode, "append") != 0 && strcmp(mode, "alias") != 0)
        return 2;
    char output[2048] = "unchanged";
    if (strcmp(mode, "alias") == 0)
    {
        strcpy(output, input);
        input = output;
    }
    UmiStatus status = UmiCompilationDatabaseClangdArguments(
        input, strcmp(mode, "relative") == 0 ? "build" : DATABASE_ROOT "/build caf\xc3\xa9", output,
        strcmp(mode, "small") == 0 ? 3U : sizeof output);
    CHECK(status == expected);
    if (status != UMI_STATUS_OK)
        CHECK(strcmp(output, "unchanged") == 0);
    else
    {
        UmiLanguageRuntimeArguments *parsed = malloc(sizeof *parsed);
        CHECK(parsed != NULL);
        CHECK(umi_language_runtime_arguments_parse(output, parsed) == UMI_STATUS_OK);
        CHECK(parsed->count == 2U);
        bool found = false, other = false;
        for (size_t i = 0U; i < parsed->count; ++i)
        {
            if (strcmp(parsed->values[i],
                       "--compile-commands-dir=" DATABASE_ROOT "/build caf\xc3\xa9") == 0)
                found = true;
            if (strcmp(parsed->values[i], "--background-index") == 0)
                other = true;
        }
        CHECK(found && other);
        free(parsed);
    }
    return EXIT_SUCCESS;
}
