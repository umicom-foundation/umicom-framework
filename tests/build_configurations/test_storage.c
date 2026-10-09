/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build_configurations/test_storage.c
 * PURPOSE: Exercise coherent named settings, replacement, stale writers and current-profile independence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    UmiDataServer *server = NULL;
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    UmiBuildProfile *profile = malloc(sizeof *profile), *loaded = malloc(sizeof *loaded);
    CHECK(profile != NULL && loaded != NULL);
    ConfigurationFixtureProfile(profile);
    UmiBuildConfigurationCatalogue catalogue;
    CHECK(UmiBuildConfigurationCapture(server, CONFIGURATION_ROOT, &catalogue) == UMI_STATUS_OK);
    CHECK(catalogue.count == 0U && catalogue.revision == 0U);
    uint64_t revision = 0U, active_revision = 0U;
    CHECK(UmiBuildProfileStoreSave(server, profile, 0U, &active_revision) == UMI_STATUS_OK);
    CHECK(UmiBuildConfigurationSave(server, "Debug samples", profile, 0U, &revision) ==
          UMI_STATUS_OK);
    CHECK(revision == 1U);
    if (strcmp(argv[1], "roundtrip") == 0)
    {
        CHECK(UmiBuildConfigurationLoad(server, CONFIGURATION_ROOT, "Debug samples", revision,
                                        loaded) == UMI_STATUS_OK);
        CHECK(umi_build_profile_equal(profile, loaded));
        strcpy(profile->configuration, "Release");
        strcpy(profile->run_environment, "APP_MODE=release");
        CHECK(UmiBuildConfigurationSave(server, "Release", profile, revision, &revision) ==
              UMI_STATUS_OK);
        CHECK(UmiBuildConfigurationCapture(server, CONFIGURATION_ROOT, &catalogue) ==
              UMI_STATUS_OK);
        CHECK(catalogue.count == 2U && catalogue.revision == revision);
        CHECK(UmiBuildConfigurationLoad(server, CONFIGURATION_ROOT, "Release", revision, loaded) ==
              UMI_STATUS_OK);
        CHECK(umi_build_profile_equal(profile, loaded));
        CHECK(UmiBuildProfileStoreLoad(server, CONFIGURATION_ROOT, loaded, &active_revision) ==
              UMI_STATUS_OK);
        CHECK(strcmp(loaded->configuration, "Debug") == 0);
        CHECK(UmiBuildConfigurationCapture(server, CONFIGURATION_OTHER, &catalogue) ==
                  UMI_STATUS_OK &&
              catalogue.count == 0U);
    }
    else if (strcmp(argv[1], "stale") == 0)
    {
        uint64_t untouched = 555U;
        CHECK(UmiBuildConfigurationSave(server, "Other", profile, 0U, &untouched) ==
              UMI_STATUS_INVALID_STATE);
        CHECK(untouched == 555U);
        *loaded = *profile;
        CHECK(UmiBuildConfigurationLoad(server, CONFIGURATION_ROOT, "Debug samples", 0U, loaded) ==
              UMI_STATUS_INVALID_STATE);
        CHECK(umi_build_profile_equal(profile, loaded));
        CHECK(UmiBuildConfigurationCapture(server, CONFIGURATION_ROOT, &catalogue) ==
                  UMI_STATUS_OK &&
              catalogue.count == 1U);
    }
    else if (strcmp(argv[1], "replace") == 0)
    {
        strcpy(profile->configuration, "RelWithDebInfo");
        CHECK(UmiBuildConfigurationSave(server, "Debug samples", profile, revision, &revision) ==
              UMI_STATUS_OK);
        CHECK(revision == 2U);
        CHECK(UmiBuildConfigurationCapture(server, CONFIGURATION_ROOT, &catalogue) ==
                  UMI_STATUS_OK &&
              catalogue.count == 1U);
        CHECK(UmiBuildConfigurationLoad(server, CONFIGURATION_ROOT, "Debug samples", revision,
                                        loaded) == UMI_STATUS_OK);
        CHECK(umi_build_profile_equal(profile, loaded));
    }
    else if (strcmp(argv[1], "capacity") == 0)
    {
        for (size_t index = 1U; index < UMI_BUILD_CONFIGURATION_CAPACITY; ++index)
        {
            char name[32];
            snprintf(name, sizeof name, "Configuration %zu", index);
            CHECK(UmiBuildConfigurationSave(server, name, profile, revision, &revision) ==
                  UMI_STATUS_OK);
        }
        uint64_t before = revision;
        CHECK(UmiBuildConfigurationSave(server, "Overflow", profile, revision, &revision) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(revision == before);
        CHECK(UmiBuildConfigurationSave(server, "Debug samples", profile, revision, &revision) ==
              UMI_STATUS_OK);
    }
    else if (strcmp(argv[1], "nested") == 0)
    {
        CHECK(umi_data_server_begin(server) == UMI_STATUS_OK);
        CHECK(UmiBuildConfigurationSave(server, "Other", profile, revision, &revision) !=
              UMI_STATUS_OK);
        CHECK(umi_data_server_in_transaction(server));
        CHECK(umi_data_server_rollback(server) == UMI_STATUS_OK);
        CHECK(UmiBuildConfigurationCapture(server, CONFIGURATION_ROOT, &catalogue) ==
                  UMI_STATUS_OK &&
              catalogue.count == 1U);
    }
    else
    {
        CHECK(strcmp(argv[1], "names") == 0);
        const char *bad[] = {
            "",           " trailing", "trailing ", "folder/name", "line\nbreak", "quote'entry",
            "caf\xc3\xa9"};
        for (size_t index = 0U; index < sizeof bad / sizeof bad[0]; ++index)
            CHECK(UmiBuildConfigurationSave(server, bad[index], profile, revision, &revision) ==
                  UMI_STATUS_INVALID_ARGUMENT);
        char limit[65];
        memset(limit, 'a', sizeof limit);
        limit[63] = '\0';
        CHECK(UmiBuildConfigurationNameValidate(limit) == UMI_STATUS_OK);
        limit[63] = 'a';
        limit[64] = '\0';
        CHECK(UmiBuildConfigurationNameValidate(limit) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiBuildConfigurationSave(server, "debug samples", profile, revision, &revision) ==
              UMI_STATUS_OK);
        CHECK(UmiBuildConfigurationCapture(server, CONFIGURATION_ROOT, &catalogue) ==
                  UMI_STATUS_OK &&
              catalogue.count == 2U);
    }
    free(loaded);
    free(profile);
    umi_data_server_destroy(server);
    return 0;
}
