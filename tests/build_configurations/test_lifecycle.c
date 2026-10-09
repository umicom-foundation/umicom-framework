/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build_configurations/test_lifecycle.c
 * PURPOSE: Verify configuration rename, atomic removal, capacity reuse and stale selections.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include <inttypes.h>
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiDataServer *server = NULL;
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    UmiBuildProfile *profile = malloc(sizeof *profile), *loaded = malloc(sizeof *loaded);
    CHECK(profile != NULL && loaded != NULL);
    ConfigurationFixtureProfile(profile);
    uint64_t active_revision = 0U, revision = 0U, result = 777U;
    CHECK(UmiBuildProfileStoreSave(server, profile, 0U, &active_revision) == UMI_STATUS_OK);
    const char *names[] = {"First", "Middle", "Last"};
    for (size_t index = 0U; index < 3U; ++index)
    {
        snprintf(profile->run_environment, sizeof profile->run_environment, "CHOICE=%zu", index);
        CHECK(UmiBuildConfigurationSave(server, names[index], profile, revision, &revision) ==
              UMI_STATUS_OK);
    }
    UmiBuildConfigurationCatalogue catalogue;
    if (strcmp(mode, "rename") == 0)
    {
        CHECK(UmiBuildConfigurationRename(server, CONFIGURATION_ROOT, "Middle", "Reviewed",
                                          revision, &result) == UMI_STATUS_OK);
        CHECK(result == revision + 1U);
        CHECK(UmiBuildConfigurationLoad(server, CONFIGURATION_ROOT, "Reviewed", result, loaded) ==
              UMI_STATUS_OK);
        CHECK(strcmp(loaded->run_environment, "CHOICE=1") == 0);
        CHECK(UmiBuildConfigurationLoad(server, CONFIGURATION_ROOT, "Middle", result, loaded) ==
              UMI_STATUS_NOT_FOUND);
    }
    else if (strcmp(mode, "duplicate") == 0)
    {
        CHECK(UmiBuildConfigurationRename(server, CONFIGURATION_ROOT, "First", "Last", revision,
                                          &result) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(result == 777U);
    }
    else if (strcmp(mode, "same") == 0)
    {
        CHECK(UmiBuildConfigurationRename(server, CONFIGURATION_ROOT, "First", "First", revision,
                                          &result) == UMI_STATUS_OK);
        CHECK(result == revision);
    }
    else if (strcmp(mode, "stale") == 0)
    {
        CHECK(UmiBuildConfigurationRemove(server, CONFIGURATION_ROOT, "First", revision - 1U,
                                          &result) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiBuildConfigurationRename(server, CONFIGURATION_ROOT, "First", "Changed",
                                          revision - 1U, &result) == UMI_STATUS_INVALID_STATE);
        CHECK(result == 777U);
    }
    else if (strcmp(mode, "missing") == 0)
    {
        CHECK(UmiBuildConfigurationRemove(server, CONFIGURATION_ROOT, "Absent", revision,
                                          &result) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiBuildConfigurationRename(server, CONFIGURATION_ROOT, "Absent", "Changed", revision,
                                          &result) == UMI_STATUS_NOT_FOUND);
        CHECK(result == 777U);
    }
    else if (strcmp(mode, "nested") == 0)
    {
        CHECK(umi_data_server_begin(server) == UMI_STATUS_OK);
        CHECK(UmiBuildConfigurationRemove(server, CONFIGURATION_ROOT, "First", revision, &result) !=
              UMI_STATUS_OK);
        CHECK(UmiBuildConfigurationRename(server, CONFIGURATION_ROOT, "First", "Changed", revision,
                                          &result) != UMI_STATUS_OK);
        CHECK(result == 777U && umi_data_server_in_transaction(server));
        CHECK(umi_data_server_rollback(server) == UMI_STATUS_OK);
    }
    else if (strcmp(mode, "unknown") == 0 || strcmp(mode, "damaged") == 0 ||
             strcmp(mode, "overflow") == 0)
    {
        char key[192], number[32];
        const char *field = strcmp(mode, "unknown") == 0   ? "entry.2.future_setting"
                            : strcmp(mode, "damaged") == 0 ? "entry.2.schema"
                                                           : "revision";
        snprintf(number, sizeof number, "%" PRIu64, UINT64_MAX);
        ConfigurationFixtureKey(field, key);
        CHECK(umi_data_server_set(server, key,
                                  strcmp(mode, "overflow") == 0 ? number : "unrecognised") ==
              UMI_STATUS_OK);
        uint64_t expected = strcmp(mode, "overflow") == 0 ? UINT64_MAX : revision;
        CHECK(UmiBuildConfigurationRemove(server, CONFIGURATION_ROOT, "First", expected, &result) !=
              UMI_STATUS_OK);
        CHECK(result == 777U);
        CHECK(UmiBuildConfigurationCapture(server, CONFIGURATION_ROOT, &catalogue) ==
              UMI_STATUS_OK);
        CHECK(catalogue.count == 3U && strcmp(catalogue.names[0], "First") == 0);
        char retained[80];
        CHECK(umi_data_server_get(server, key, retained, sizeof retained) == UMI_STATUS_OK);
        CHECK(strcmp(retained, strcmp(mode, "overflow") == 0 ? number : "unrecognised") == 0);
    }
    else if (strcmp(mode, "reuse") == 0)
    {
        for (size_t index = 3U; index < UMI_BUILD_CONFIGURATION_CAPACITY; ++index)
        {
            char name[32];
            snprintf(name, sizeof name, "Saved %zu", index);
            CHECK(UmiBuildConfigurationSave(server, name, profile, revision, &revision) ==
                  UMI_STATUS_OK);
        }
        CHECK(UmiBuildConfigurationRemove(server, CONFIGURATION_ROOT, "Middle", revision,
                                          &revision) == UMI_STATUS_OK);
        CHECK(UmiBuildConfigurationSave(server, "Replacement", profile, revision, &revision) ==
              UMI_STATUS_OK);
        CHECK(UmiBuildConfigurationCapture(server, CONFIGURATION_ROOT, &catalogue) ==
              UMI_STATUS_OK);
        CHECK(catalogue.count == UMI_BUILD_CONFIGURATION_CAPACITY);
    }
    else if (strcmp(mode, "empty") == 0)
    {
        for (size_t index = 0U; index < 3U; ++index)
            CHECK(UmiBuildConfigurationRemove(server, CONFIGURATION_ROOT, names[index], revision,
                                              &revision) == UMI_STATUS_OK);
        CHECK(UmiBuildConfigurationCapture(server, CONFIGURATION_ROOT, &catalogue) ==
                  UMI_STATUS_OK &&
              catalogue.count == 0U);
        CHECK(catalogue.revision == revision && revision == 6U);
        CHECK(UmiBuildConfigurationSave(server, "Again", profile, revision, &revision) ==
              UMI_STATUS_OK);
    }
    else
    {
        size_t slot = strcmp(mode, "first") == 0 ? 0U : strcmp(mode, "middle") == 0 ? 1U : 2U;
        CHECK(slot != 2U || strcmp(mode, "last") == 0);
        CHECK(UmiBuildConfigurationRemove(server, CONFIGURATION_ROOT, names[slot], revision,
                                          &result) == UMI_STATUS_OK);
        CHECK(UmiBuildConfigurationCapture(server, CONFIGURATION_ROOT, &catalogue) ==
              UMI_STATUS_OK);
        CHECK(catalogue.count == 2U && catalogue.revision == result && result == revision + 1U);
        size_t row = 0U;
        for (size_t index = 0U; index < 3U; ++index)
            if (index != slot)
            {
                CHECK(strcmp(catalogue.names[row++], names[index]) == 0);
                CHECK(UmiBuildConfigurationLoad(server, CONFIGURATION_ROOT, names[index], result,
                                                loaded) == UMI_STATUS_OK);
                char expected[32];
                snprintf(expected, sizeof expected, "CHOICE=%zu", index);
                CHECK(strcmp(loaded->run_environment, expected) == 0);
            }
    }
    /* Saved-library mutations never replace the active project profile. */
    CHECK(UmiBuildProfileStoreLoad(server, CONFIGURATION_ROOT, loaded, &active_revision) ==
          UMI_STATUS_OK);
    CHECK(strcmp(loaded->run_environment, "APP_MODE=review DATA_FOLDER='sample data'") == 0);
    free(loaded);
    free(profile);
    umi_data_server_destroy(server);
    return 0;
}
