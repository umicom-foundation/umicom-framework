/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build_configurations/test_lifecycle_sqlite.c
 * PURPOSE: Verify rollback after partial removal and visibility across independent SQLite connections.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/platform/filesystem.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    bool remove_failure = strcmp(argv[1], "remove-failure") == 0;
    bool rename_failure = strcmp(argv[1], "rename-failure") == 0;
    CHECK(remove_failure || rename_failure || strcmp(argv[1], "reopen") == 0);
    const char *path = "configuration-lifecycle.sqlite";
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
    uint64_t revision = 0U;
    CHECK(UmiBuildConfigurationSave(first, "First", profile, revision, &revision) == UMI_STATUS_OK);
    strcpy(profile->run_environment, "APP_MODE=retained");
    CHECK(UmiBuildConfigurationSave(first, "Second", profile, revision, &revision) ==
          UMI_STATUS_OK);
    if (remove_failure || rename_failure)
    {
        CHECK(umi_data_server_execute(
                  first, remove_failure
                             ? "CREATE TRIGGER reject_lifecycle BEFORE DELETE ON umicom_kv "
                               "WHEN OLD.key LIKE '%.configurations.entry.1.run_environment' "
                               "BEGIN SELECT RAISE(ABORT,'removal failure'); END;"
                             : "CREATE TRIGGER reject_lifecycle BEFORE INSERT ON umicom_kv "
                               "WHEN NEW.key LIKE '%.configurations.revision' "
                               "BEGIN SELECT RAISE(ABORT,'revision failure'); END;") ==
              UMI_STATUS_OK);
        uint64_t untouched = 999U;
        status = remove_failure ? UmiBuildConfigurationRemove(first, CONFIGURATION_ROOT, "First",
                                                              revision, &untouched)
                                : UmiBuildConfigurationRename(first, CONFIGURATION_ROOT, "First",
                                                              "Changed", revision, &untouched);
        CHECK(status != UMI_STATUS_OK && untouched == 999U);
        CHECK(umi_data_server_execute(first, "DROP TRIGGER reject_lifecycle;") == UMI_STATUS_OK);
        UmiBuildConfigurationCatalogue catalogue;
        CHECK(UmiBuildConfigurationCapture(second, CONFIGURATION_ROOT, &catalogue) ==
              UMI_STATUS_OK);
        CHECK(catalogue.revision == revision && catalogue.count == 2U &&
              strcmp(catalogue.names[0], "First") == 0);
    }
    CHECK(UmiBuildConfigurationRemove(first, CONFIGURATION_ROOT, "First", revision, &revision) ==
          UMI_STATUS_OK);
    uint64_t stale = 555U;
    CHECK(UmiBuildConfigurationRename(second, CONFIGURATION_ROOT, "Second", "Stale", revision - 1U,
                                      &stale) == UMI_STATUS_INVALID_STATE);
    CHECK(stale == 555U);
    CHECK(UmiBuildConfigurationRename(second, CONFIGURATION_ROOT, "Second", "Renamed", revision,
                                      &revision) == UMI_STATUS_OK);
    umi_data_server_destroy(first);
    umi_data_server_destroy(second);
    CHECK(umi_data_server_create_sqlite(path, &first) == UMI_STATUS_OK);
    UmiBuildConfigurationCatalogue catalogue;
    CHECK(UmiBuildConfigurationCapture(first, CONFIGURATION_ROOT, &catalogue) == UMI_STATUS_OK);
    CHECK(catalogue.count == 1U && catalogue.revision == revision &&
          strcmp(catalogue.names[0], "Renamed") == 0);
    CHECK(UmiBuildConfigurationLoad(first, CONFIGURATION_ROOT, "Renamed", revision, loaded) ==
          UMI_STATUS_OK);
    CHECK(umi_build_profile_equal(profile, loaded));
    umi_data_server_destroy(first);
    free(profile);
    free(loaded);
    CHECK(umi_fs_remove_tree(path) == UMI_STATUS_OK);
    return 0;
}
