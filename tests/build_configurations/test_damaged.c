/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build_configurations/test_damaged.c
 * PURPOSE: Preserve partial or inconsistent configuration records rather than replacing them as absent.
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
    *loaded = *profile;
    uint64_t revision = 0U;
    CHECK(UmiBuildConfigurationSave(server, "Debug", profile, 0U, &revision) == UMI_STATUS_OK);
    char key[192];
    bool record_only = false;
    const char *field = "schema", *value = "unsupported";
    if (strcmp(argv[1], "missing-marker") == 0)
        value = NULL;
    else if (strcmp(argv[1], "wrong-root") == 0)
    {
        field = "source";
        value = CONFIGURATION_OTHER;
    }
    else if (strcmp(argv[1], "bad-count") == 0)
    {
        field = "count";
        value = "33";
    }
    else if (strcmp(argv[1], "bad-revision") == 0)
    {
        field = "revision";
        value = "-1";
    }
    else if (strcmp(argv[1], "orphan-name") == 0)
    {
        field = "entry.2.name";
        value = "Orphan";
    }
    else if (strcmp(argv[1], "missing-field") == 0)
    {
        field = "entry.0.run_environment";
        value = NULL;
        record_only = true;
    }
    else if (strcmp(argv[1], "future-entry") == 0)
    {
        field = "entry.0.revision";
        value = "99";
        record_only = true;
    }
    else
        CHECK(strcmp(argv[1], "unsupported") == 0);
    ConfigurationFixtureKey(field, key);
    CHECK((value != NULL ? umi_data_server_set(server, key, value)
                         : umi_data_server_delete(server, key)) == UMI_STATUS_OK);
    UmiBuildConfigurationCatalogue before = {0}, catalogue = {0};
    before.revision = 444U;
    catalogue = before;
    if (!record_only)
    {
        CHECK(UmiBuildConfigurationCapture(server, CONFIGURATION_ROOT, &catalogue) !=
              UMI_STATUS_OK);
        CHECK(before.revision == catalogue.revision && before.count == catalogue.count &&
              memcmp(before.names, catalogue.names, sizeof before.names) == 0);
    }
    CHECK(UmiBuildConfigurationLoad(server, CONFIGURATION_ROOT, "Debug", 1U, loaded) !=
          UMI_STATUS_OK);
    CHECK(umi_build_profile_equal(profile, loaded));
    uint64_t untouched = 555U;
    CHECK(UmiBuildConfigurationSave(server, "Debug", profile, 1U, &untouched) != UMI_STATUS_OK &&
          untouched == 555U);
    char retained[4096];
    UmiStatus read = umi_data_server_get(server, key, retained, sizeof retained);
    CHECK(value != NULL ? read == UMI_STATUS_OK && strcmp(retained, value) == 0
                        : read == UMI_STATUS_NOT_FOUND);
    free(loaded);
    free(profile);
    umi_data_server_destroy(server);
    return 0;
}
