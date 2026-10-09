/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build_configurations/test_absolute_identity.c
 * PURPOSE: Keep absolute project settings usable when the process directory disappears.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#define _POSIX_C_SOURCE 200809L
#include "fixture.h"
#ifndef _WIN32
#include <sys/stat.h>
#include <unistd.h>
#endif
int main(void)
{
#ifdef _WIN32
    /* Windows prevents removal of the active directory; POSIX supplies the
     * disappearing-cwd condition this regression specifically exercises. */
    return 77;
#else
    char original[UMI_PATH_CAPACITY];
    CHECK(getcwd(original, sizeof original) != NULL);
    char temporary[] = "/tmp/umicom-profile-identity-XXXXXX";
    CHECK(mkdtemp(temporary) != NULL);
    CHECK(chdir(temporary) == 0);
    CHECK(rmdir(temporary) == 0);
    char unavailable[UMI_PATH_CAPACITY];
    CHECK(getcwd(unavailable, sizeof unavailable) == NULL);
    UmiDataServer *server = NULL;
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    UmiBuildProfile *profile = malloc(sizeof *profile);
    CHECK(profile != NULL);
    ConfigurationFixtureProfile(profile);
    uint64_t revision = 0U;
    CHECK(UmiBuildConfigurationSave(server, "Absolute", profile, 0U, &revision) == UMI_STATUS_OK);
    CHECK(UmiBuildConfigurationRename(server, CONFIGURATION_ROOT, "Absolute", "Independent",
                                      revision, &revision) == UMI_STATUS_OK);
    CHECK(UmiBuildConfigurationRemove(server, CONFIGURATION_ROOT, "Independent", revision,
                                      &revision) == UMI_STATUS_OK);
    CHECK(chdir(original) == 0);
    free(profile);
    umi_data_server_destroy(server);
    return 0;
#endif
}
