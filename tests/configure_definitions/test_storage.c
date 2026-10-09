/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/configure_definitions/test_storage.c
 * PURPOSE: Check configure-option persistence, migration and stale-writer rejection through the Data Server.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/build/profile_store.h"
#include "umicom/platform/recent_items.h"
#include "umicom/platform/path.h"

static void Key(const char *field, char out[192])
{
    char prefix[128], normalised[UMI_BUILD_PATH_CAPACITY];
    CHECK(umi_path_normalise(PROJECT_ROOT, normalised, sizeof normalised) == UMI_STATUS_OK);
#ifdef _WIN32
    for (size_t index = 0U; normalised[index] != '\0'; ++index)
        if (normalised[index] >= 'A' && normalised[index] <= 'Z')
            normalised[index] = (char)(normalised[index] + ('a' - 'A'));
    const char *scope = "build-profile-windows";
#else
    const char *scope = "build-profile-posix";
#endif
    CHECK(umi_platform_recent_item_id_from_uri(scope, normalised, prefix, sizeof prefix) ==
          UMI_STATUS_OK);
    int count = snprintf(out, 192U, "%s.%s", prefix, field);
    CHECK(count > 0 && count < 192);
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    UmiDataServer *server = NULL;
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    UmiBuildProfile profile, loaded, before;
    Profile(&profile);
    uint64_t revision = 0U;
    CHECK(UmiBuildProfileStoreSave(server, &profile, 0U, &revision) == UMI_STATUS_OK);
    char key[192], schema[192];
    Key("configure_definitions", key);
    Key("schema", schema);
    const char *mode = argv[1];
    CHECK(UmiBuildProfileStoreLoad(server, PROJECT_ROOT, &loaded, &revision) == UMI_STATUS_OK);
    before = loaded;
    if (strcmp(mode, "migration") == 0)
    {
        for (unsigned old = 1U; old <= 4U; ++old)
        {
            char marker[16];
            (void)snprintf(marker, sizeof marker, "%u", old);
            CHECK(umi_data_server_delete(server, key) == UMI_STATUS_OK);
            CHECK(umi_data_server_set(server, schema, marker) == UMI_STATUS_OK);
            CHECK(UmiBuildProfileStoreLoad(server, PROJECT_ROOT, &loaded, &revision) ==
                  UMI_STATUS_OK);
            CHECK(loaded.configure_definitions[0] == '\0');
            CHECK(UmiBuildProfileStoreSave(server, &loaded, revision, &revision) == UMI_STATUS_OK);
            CHECK(umi_data_server_get(server, schema, marker, sizeof marker) == UMI_STATUS_OK);
/* The current profile writer also publishes launch variables. Retain the preceding marker assertion for migration review. The previous implementation is retained for engineering review. */
#if 0
            CHECK(strcmp(marker, "5") == 0);
#endif
            CHECK(strcmp(marker, "6") == 0);
        }
    }
    else if (strcmp(mode, "missing") == 0 || strcmp(mode, "damaged") == 0)
    {
        if (strcmp(mode, "missing") == 0)
            CHECK(umi_data_server_delete(server, key) == UMI_STATUS_OK);
        else
            CHECK(umi_data_server_set(server, key, "--build other") == UMI_STATUS_OK);
        CHECK(UmiBuildProfileStoreLoad(server, PROJECT_ROOT, &loaded, &revision) ==
              UMI_STATUS_PARSE_ERROR);
        CHECK(revision == 1U && memcmp(&before, &loaded, sizeof loaded) == 0);
        CHECK(UmiBuildProfileStoreSave(server, &profile, revision, &revision) ==
              UMI_STATUS_PARSE_ERROR);
    }
    else
    {
        strcpy(profile.configure_definitions,
               "-DCMAKE_PREFIX_PATH=\"C:/SDK Files\" -DLOCAL_FEATURE=ON");
        CHECK(UmiBuildProfileStoreSave(server, &profile, revision, &revision) == UMI_STATUS_OK);
        CHECK(revision == 2U);
        CHECK(UmiBuildProfileStoreLoad(server, PROJECT_ROOT, &loaded, &revision) == UMI_STATUS_OK);
        CHECK(umi_build_profile_equal(&profile, &loaded));
        before = loaded;
        if (strcmp(mode, "downgrade") == 0)
        {
            CHECK(umi_data_server_set(server, schema, "4") == UMI_STATUS_OK);
            CHECK(UmiBuildProfileStoreLoad(server, PROJECT_ROOT, &loaded, &revision) ==
                  UMI_STATUS_PARSE_ERROR);
            CHECK(memcmp(&before, &loaded, sizeof loaded) == 0 && revision == 2U);
        }
        else
        {
            CHECK(strcmp(mode, "roundtrip") == 0);
            strcpy(profile.configure_definitions, "-DLOCAL_FEATURE=OFF");
            CHECK(UmiBuildProfileStoreSave(server, &profile, 1U, &revision) ==
                  UMI_STATUS_INVALID_STATE);
            CHECK(revision == 2U);
            CHECK(UmiBuildProfileStoreLoad(server, PROJECT_ROOT, &loaded, &revision) ==
                  UMI_STATUS_OK);
            CHECK(umi_build_profile_equal(&before, &loaded));
            CHECK(UmiBuildProfileStoreSave(server, &profile, revision, &revision) == UMI_STATUS_OK);
            CHECK(UmiBuildProfileStoreLoad(server, PROJECT_ROOT, &loaded, &revision) ==
                  UMI_STATUS_OK);
            CHECK(umi_build_profile_equal(&profile, &loaded) && revision == 3U);
        }
    }
    umi_data_server_destroy(server);
    return 0;
}
