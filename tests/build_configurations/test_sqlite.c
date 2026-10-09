/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build_configurations/test_sqlite.c
 * PURPOSE: Verify local SQLite persistence and stale writes across independently opened connections.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/platform/filesystem.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *path = "configuration-library.sqlite";
    CHECK(!umi_fs_exists(path));
    UmiDataServer *first = NULL, *second = NULL;
    UmiStatus status = umi_data_server_create_sqlite(path, &first);
    if (status == UMI_STATUS_UNAVAILABLE)
        return 77;
    CHECK(status == UMI_STATUS_OK);
    CHECK(umi_data_server_create_sqlite(path, &second) == UMI_STATUS_OK);
    UmiBuildProfile *profile = malloc(sizeof *profile), *loaded = malloc(sizeof *loaded);
    CHECK(profile != NULL && loaded != NULL);
    ConfigurationFixtureProfile(profile);
    UmiBuildConfigurationCatalogue old;
    CHECK(UmiBuildConfigurationCapture(second, CONFIGURATION_ROOT, &old) == UMI_STATUS_OK &&
          old.revision == 0U);
    uint64_t revision = 0U;
    CHECK(UmiBuildConfigurationSave(first, "Debug", profile, 0U, &revision) == UMI_STATUS_OK);
    CHECK(UmiBuildConfigurationSave(second, "Release", profile, old.revision, &revision) ==
          UMI_STATUS_INVALID_STATE);
    if (strcmp(argv[1], "rollback") == 0)
    {
        /* Reject a later field after preceding values have been written. The
         * transaction must preserve both the old profile and catalogue. */
        CHECK(umi_data_server_execute(
                  first, "CREATE TRIGGER reject_configuration BEFORE INSERT ON umicom_kv "
                         "WHEN NEW.key LIKE '%.configurations.entry.0.run_environment' "
                         "BEGIN SELECT RAISE(ABORT,'configuration fixture failure'); END;") ==
              UMI_STATUS_OK);
        strcpy(profile->configuration, "Release");
        uint64_t unchanged = 777U;
        CHECK(UmiBuildConfigurationSave(first, "Debug", profile, 1U, &unchanged) != UMI_STATUS_OK);
        CHECK(unchanged == 777U);
        CHECK(umi_data_server_execute(first, "DROP TRIGGER reject_configuration;") ==
              UMI_STATUS_OK);
        CHECK(UmiBuildConfigurationLoad(second, CONFIGURATION_ROOT, "Debug", 1U, loaded) ==
              UMI_STATUS_OK);
        CHECK(strcmp(loaded->configuration, "Debug") == 0);
        strcpy(profile->configuration, "Debug");
    }
    else
        CHECK(strcmp(argv[1], "reopen") == 0);
    umi_data_server_destroy(first);
    umi_data_server_destroy(second);
    first = NULL;
    second = NULL;
    CHECK(umi_data_server_create_sqlite(path, &first) == UMI_STATUS_OK);
    CHECK(UmiBuildConfigurationCapture(first, CONFIGURATION_ROOT, &old) == UMI_STATUS_OK &&
          old.count == 1U);
    CHECK(UmiBuildConfigurationLoad(first, CONFIGURATION_ROOT, "Debug", old.revision, loaded) ==
          UMI_STATUS_OK);
    CHECK(umi_build_profile_equal(profile, loaded));
    umi_data_server_destroy(first);
    free(loaded);
    free(profile);
    CHECK(umi_fs_remove_tree(path) == UMI_STATUS_OK);
    return 0;
}
