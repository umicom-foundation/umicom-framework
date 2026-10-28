/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_reuse/test_provider.c
 *
 * PURPOSE:
 *   Verify generated driver discovery and preservation of preset-owned test commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/ctest_provider.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    UmiBuildProfile *profile = malloc(sizeof(*profile)); CHECK(profile != NULL);
    CHECK(umi_build_profile_set(profile, "automatic", argv[1], argv[1]) == UMI_STATUS_OK);
    char driver[UMI_BUILD_PATH_CAPACITY];
    CHECK(UmiBuildAutomaticTestDriver(profile, driver, sizeof(driver)) == UMI_STATUS_OK);
    CHECK(strstr(driver, "automatic-tests-Debug.cmake") != NULL);
    UmiBuildProvider provider = UmiBuildAutomaticTestProvider();
    UmiBuildCommand command;
    CHECK(umi_build_provider_create_command(&provider, profile, UMI_BUILD_PHASE_TEST, &command) == UMI_STATUS_OK);
    char formatted[UMI_BUILD_COMMAND_CAPACITY];
    CHECK(umi_build_command_format(&command, formatted, sizeof(formatted)));
    CHECK(strstr(formatted, "automatic-tests-Debug.cmake") != NULL);
    CHECK(strstr(formatted, "-DUMICOM_AUTOMATIC_CONFIGURATION=Debug") != NULL);
    strcpy(profile->test_preset, "owner-selected");
    CHECK(umi_build_provider_create_command(&provider, profile, UMI_BUILD_PHASE_TEST, &command) == UMI_STATUS_OK);
    CHECK(umi_build_command_format(&command, formatted, sizeof(formatted)));
    CHECK(strstr(formatted, "--preset") != NULL && strstr(formatted, "owner-selected") != NULL);
    CHECK(UmiBuildAutomaticTestDriver(profile, driver, sizeof(driver)) == UMI_STATUS_NOT_FOUND);
    CHECK(strstr(formatted, "automatic-tests") == NULL);
    free(profile); return 0;
}
