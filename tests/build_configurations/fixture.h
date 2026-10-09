/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build_configurations/fixture.h
 * PURPOSE: Share isolated project settings and key inspection helpers for configuration tests.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_CONFIGURATIONS_TEST_FIXTURE_H
#define UMICOM_BUILD_CONFIGURATIONS_TEST_FIXTURE_H
#include "umicom/build/configuration_library.h"
#include "umicom/platform/path.h"
#include "umicom/platform/recent_items.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value)                                                                               \
    do                                                                                             \
    {                                                                                              \
        if (!(value))                                                                              \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                            \
            exit(1);                                                                               \
        }                                                                                          \
    } while (0)
#ifdef _WIN32
#define CONFIGURATION_ROOT "C:/Umicom Configuration Tests"
#define CONFIGURATION_OTHER "C:/Another Project"
#else
#define CONFIGURATION_ROOT "/umicom-configuration-tests"
#define CONFIGURATION_OTHER "/another-project"
#endif
static inline void ConfigurationFixtureProfile(UmiBuildProfile *profile)
{
    umi_build_profile_init(profile);
    strcpy(profile->source_directory, CONFIGURATION_ROOT);
    strcpy(profile->run_program, "./build/debug/hello");
    strcpy(profile->run_arguments, "'hello caf\xc3\xa9' --mode review");
    strcpy(profile->run_environment, "APP_MODE=review DATA_FOLDER='sample data'");
    strcpy(profile->configure_definitions, "-DPROJECT_FEATURE:BOOL=ON");
    strcpy(profile->run_working_directory, "sample data");
}
static inline void ConfigurationFixtureKey(const char *field, char out[192])
{
    char identity[UMI_BUILD_PATH_CAPACITY], prefix[128];
    CHECK(umi_path_normalise(CONFIGURATION_ROOT, identity, sizeof identity) == UMI_STATUS_OK);
#ifdef _WIN32
    for (size_t index = 0U; identity[index] != '\0'; ++index)
        if (identity[index] >= 'A' && identity[index] <= 'Z')
            identity[index] = (char)(identity[index] + ('a' - 'A'));
    const char *scope = "build-profile-windows";
#else
    const char *scope = "build-profile-posix";
#endif
    CHECK(umi_platform_recent_item_id_from_uri(scope, identity, prefix, sizeof prefix) ==
          UMI_STATUS_OK);
    int length = snprintf(out, 192U, "%s.configurations.%s", prefix, field);
    CHECK(length > 0 && length < 192);
}
#endif
