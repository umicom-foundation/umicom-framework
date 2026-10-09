/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/configure_definitions/test_syntax.c
 * PURPOSE: Check literal configure definitions, malformed fields and profile-owned option boundaries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    UmiBuildProfile profile;
    Profile(&profile);
    UmiArguments *parsed = malloc(sizeof *parsed);
    CHECK(parsed != NULL);
    const char *mode = argv[1];
    if (strcmp(mode, "values") == 0)
    {
        strcpy(profile.configure_definitions,
               "-DCMAKE_PREFIX_PATH:PATH=\"C:/SDKs/Umicom caf\xc3\xa9;C:/Other SDK\" "
               "-DPROJECT_LABEL:STRING=\"literal $HOME & text\" -DPROJECT_EMPTY= "
               "-DPROJECT_FEATURE:BOOL=ON -DPROJECT_FILE:FILEPATH=source.c "
               "-DPROJECT_PRIVATE:INTERNAL=1");
        CHECK(UmiBuildConfigureDefinitions(&profile, parsed) == UMI_STATUS_OK);
        CHECK(parsed->count == 6U);
        CHECK(strcmp(parsed->values[0],
                     "-DCMAKE_PREFIX_PATH:PATH=C:/SDKs/Umicom caf\xc3\xa9;C:/Other SDK") == 0);
        CHECK(strcmp(parsed->values[1], "-DPROJECT_LABEL:STRING=literal $HOME & text") == 0);
        CHECK(strcmp(parsed->values[2], "-DPROJECT_EMPTY=") == 0);
        CHECK(umi_build_profile_validate(&profile, NULL, 0U) == UMI_STATUS_OK);
    }
    else if (strcmp(mode, "invalid") == 0)
    {
        const char *bad[] = {"--build",
                             "-Pscript.cmake",
                             "-D",
                             "-DNAME",
                             "-DNAME:BAD=value",
                             "-DNAME:=value",
                             "-D1NAME=value",
                             "-DName;Other=value",
                             "\"unterminated",
                             "-DNAME=\"line\nbreak\"",
                             "-DNAME=one -DNAME:STRING=two",
                             "-D NAME=value",
                             "\"\""};
        for (size_t index = 0U; index < sizeof bad / sizeof bad[0]; ++index)
        {
            strcpy(profile.configure_definitions, bad[index]);
            memset(parsed, 0x5a, sizeof *parsed);
            CHECK(UmiBuildConfigureDefinitions(&profile, parsed) != UMI_STATUS_OK);
            CHECK(parsed->count == 0U && parsed->values[0] == NULL &&
                  parsed->storage[0][0] == '\0');
            CHECK(umi_build_profile_validate(&profile, NULL, 0U) != UMI_STATUS_OK);
        }
    }
    else if (strcmp(mode, "reserved") == 0)
    {
        const char *names[] = {"CMAKE_BUILD_TYPE",    "CMAKE_C_COMPILER",
                               "BUILD_TESTING",       "UMICOM_ENABLE_STRICT_WARNINGS",
                               "CMAKE_GENERATOR",     "CMAKE_HOME_DIRECTORY",
                               "CMAKE_CACHEFILE_DIR", "CMAKE_INSTALL_PREFIX"};
        for (size_t index = 0U; index < sizeof names / sizeof names[0]; ++index)
        {
            (void)snprintf(profile.configure_definitions, sizeof profile.configure_definitions,
                           "-D%s:STRING=other", names[index]);
            CHECK(UmiBuildConfigureDefinitions(&profile, parsed) == UMI_STATUS_INVALID_ARGUMENT);
        }
    }
    else if (strcmp(mode, "bounds") == 0)
    {
        strcpy(profile.configure_definitions, "-DNAME=");
        memset(profile.configure_definitions + 7U, 'x', UMI_BUILD_ARGUMENT_CAPACITY - 8U);
        profile.configure_definitions[UMI_BUILD_ARGUMENT_CAPACITY - 1U] = '\0';
        CHECK(UmiBuildConfigureDefinitions(&profile, parsed) == UMI_STATUS_OK);
        profile.configure_definitions[UMI_BUILD_ARGUMENT_CAPACITY - 1U] = 'x';
        profile.configure_definitions[UMI_BUILD_ARGUMENT_CAPACITY] = '\0';
        CHECK(UmiBuildConfigureDefinitions(&profile, parsed) == UMI_STATUS_CAPACITY_EXCEEDED);
        memset(profile.configure_definitions, 'x', sizeof profile.configure_definitions);
        CHECK(UmiBuildConfigureDefinitions(&profile, parsed) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(parsed->count == 0U);
        CHECK(UmiBuildConfigureDefinitions(NULL, parsed) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBuildConfigureDefinitions(&profile, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    }
    else if (strcmp(mode, "count") == 0)
    {
        size_t used = 0U;
        for (unsigned index = 0U; index <= UMI_ARGUMENTS_CAPACITY; ++index)
        {
            int bytes =
                snprintf(profile.configure_definitions + used,
                         sizeof profile.configure_definitions - used, "-DOPTION_%u=1 ", index);
            CHECK(bytes > 0 && (size_t)bytes < sizeof profile.configure_definitions - used);
            used += (size_t)bytes;
            UmiStatus status = UmiBuildConfigureDefinitions(&profile, parsed);
            CHECK(index < UMI_ARGUMENTS_CAPACITY ? status == UMI_STATUS_OK
                                                 : status == UMI_STATUS_CAPACITY_EXCEEDED);
        }
    }
    else
    {
        CHECK(strcmp(mode, "empty") == 0);
        CHECK(UmiBuildConfigureDefinitions(&profile, parsed) == UMI_STATUS_OK &&
              parsed->count == 0U);
        strcpy(profile.configure_definitions, "   ");
        CHECK(UmiBuildConfigureDefinitions(&profile, parsed) == UMI_STATUS_OK &&
              parsed->count == 0U);
    }
    free(parsed);
    return 0;
}
