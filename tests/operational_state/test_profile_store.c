/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/operational_state/test_profile_store.c
 * PURPOSE: Verify project settings with real memory and SQLite Data Servers.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/build/profile_store.h"
#include "umicom/platform/clock.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/recent_items.h"

#define CHECK(test) do { if (!(test)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #test); return EXIT_FAILURE; } } while (0)

static void Profile(UmiBuildProfile *profile, const char *root)
{
    umi_build_profile_init(profile);
    (void)umi_build_profile_set(profile, "development", root, "build with spaces");
    (void)snprintf(profile->generator, sizeof(profile->generator), "%s", "Ninja");
    (void)snprintf(profile->configuration, sizeof(profile->configuration), "%s", "Debug");
    (void)snprintf(profile->compiler, sizeof(profile->compiler), "%s", "compiler with spaces");
    (void)snprintf(profile->preset, sizeof(profile->preset), "%s", "local-debug");
    (void)snprintf(profile->build_target, sizeof(profile->build_target), "%s", "umicom-notes");
    (void)snprintf(profile->run_program, sizeof(profile->run_program), "%s", "build with spaces/bin/umicom-notes");
    (void)snprintf(profile->run_argument, sizeof(profile->run_argument), "%s", "--file \"notes for review.txt\"");
    (void)snprintf(profile->install_directory, sizeof(profile->install_directory), "%s", "build/install");
    profile->parallel_jobs = 2U;
    profile->timeout_ms = 90000U;
    profile->build_testing = 1;
    profile->strict_warnings = 1;
}

/* Test-only access to documented field keys lets us inject corrupt records
 * through the Data Server, rather than reaching around it with raw SQLite. */
static int Key(const char *root, const char *field, char *out)
{
    char current[2048], normalised[2048], prefix[128];
    CHECK(umi_fs_current_directory(current, sizeof(current)) == UMI_STATUS_OK);
    CHECK(umi_path_absolute(root, current, normalised, sizeof(normalised)) == UMI_STATUS_OK);
#ifdef _WIN32
    for (size_t i = 0U; normalised[i] != '\0'; ++i)
        if (normalised[i] >= 'A' && normalised[i] <= 'Z') normalised[i] = (char)(normalised[i] + ('a' - 'A'));
    const char *scope = "build-profile-windows";
#else
    const char *scope = "build-profile-posix";
#endif
    CHECK(umi_platform_recent_item_id_from_uri(scope, normalised, prefix, sizeof(prefix)) == UMI_STATUS_OK);
    CHECK(snprintf(out, 192U, "%s.%s", prefix, field) < 192);
    return EXIT_SUCCESS;
}

static int RoundTrip(void)
{
    UmiDataServer *server = NULL;
    UmiBuildProfile original, loaded, untouched;
    uint64_t revision = 42U;
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    Profile(&original, "Umicom Notes");
    memset(&loaded, 0x5a, sizeof(loaded)); untouched = loaded;
    CHECK(UmiBuildProfileStoreLoad(server, "Umicom Notes", &loaded, &revision) == UMI_STATUS_NOT_FOUND);
    CHECK(revision == 0U && memcmp(&loaded, &untouched, sizeof(loaded)) == 0);
    CHECK(UmiBuildProfileStoreSave(server, &original, 0U, &revision) == UMI_STATUS_OK && revision == 1U);
    CHECK(UmiBuildProfileStoreLoad(server, "./Umicom Notes", &loaded, &revision) == UMI_STATUS_OK);
    CHECK(umi_path_is_absolute(loaded.source_directory));
    (void)snprintf(original.source_directory, sizeof(original.source_directory), "%s", loaded.source_directory);
    CHECK(umi_build_profile_equal(&original, &loaded));
    original.run_program[0] = '\0'; /* Empty Run is an intentional, persisted setting. */
    original.build_testing = -7; original.strict_warnings = INT_MAX;
    original.timeout_ms = UINT32_MAX;
    CHECK(UmiBuildProfileStoreSave(server, &original, 1U, &revision) == UMI_STATUS_OK && revision == 2U);
    CHECK(UmiBuildProfileStoreLoad(server, "Umicom Notes", &loaded, &revision) == UMI_STATUS_OK);
    CHECK(umi_build_profile_equal(&original, &loaded));
    CHECK(UmiBuildProfileStoreSave(server, &original, 1U, &revision) == UMI_STATUS_INVALID_STATE);
    CHECK(revision == 2U);
    CHECK(UmiBuildProfileStoreLoad(server, "Umicom Bank Training", &loaded, &revision) == UMI_STATUS_NOT_FOUND);
    CHECK(revision == 0U);
    umi_data_server_destroy(server);
    return EXIT_SUCCESS;
}

static int Corrupt(void)
{
    static const struct { const char *field; const char *value; } damage[] = {
        {"schema", "99"}, {"revision", "0"}, {"parallel_jobs", "-1"},
        {"timeout_ms", "4294967296"}, {"build_testing", "1junk"},
        {"strict_warnings", "9999999999999999999999999999999"},
        {"source_directory", "/unrelated/Umicom"}
    };
    for (size_t i = 0U; i < sizeof(damage)/sizeof(damage[0]); ++i) {
        UmiDataServer *server = NULL;
        UmiBuildProfile original, loaded, before;
        uint64_t revision = 777U;
        char key[192], stored[4096];
        CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
        Profile(&original, "Umicom Notes");
        CHECK(UmiBuildProfileStoreSave(server, &original, 0U, &revision) == UMI_STATUS_OK);
        CHECK(Key(original.source_directory, damage[i].field, key) == EXIT_SUCCESS);
        CHECK(umi_data_server_set(server, key, damage[i].value) == UMI_STATUS_OK);
        memset(&loaded, 0x43, sizeof(loaded)); before = loaded; revision = 777U;
        CHECK(UmiBuildProfileStoreLoad(server, "Umicom Notes", &loaded, &revision) != UMI_STATUS_OK);
        CHECK(revision == 777U && memcmp(&before, &loaded, sizeof(loaded)) == 0);
        CHECK(UmiBuildProfileStoreSave(server, &original, 0U, &revision) != UMI_STATUS_OK);
        CHECK(umi_data_server_get(server, key, stored, sizeof(stored)) == UMI_STATUS_OK);
        CHECK(strcmp(stored, damage[i].value) == 0);
        umi_data_server_destroy(server);
    }
    return EXIT_SUCCESS;
}

static int Boundaries(void)
{
    UmiDataServer *server = NULL;
    UmiBuildProfile original, loaded, before;
    uint64_t revision = 0U;
    char key[192], tooLong[UMI_BUILD_ARGUMENT_CAPACITY + 1U];
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    Profile(&original, "Umicom Notes");
    memset(original.run_argument, 'a', sizeof(original.run_argument)-1U);
    original.run_argument[sizeof(original.run_argument)-1U] = '\0';
    CHECK(UmiBuildProfileStoreSave(server, &original, 0U, &revision) == UMI_STATUS_OK);
    CHECK(UmiBuildProfileStoreLoad(server, "Umicom Notes", &loaded, &revision) == UMI_STATUS_OK);
    CHECK(strcmp(original.run_argument, loaded.run_argument) == 0);
    before = loaded;
    memset(original.run_argument, 'x', sizeof(original.run_argument));
    CHECK(UmiBuildProfileStoreSave(server, &original, 1U, &revision) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(revision == 1U);
    CHECK(Key("Umicom Notes", "run_argument", key) == EXIT_SUCCESS);
    memset(tooLong, 'a', sizeof(tooLong)-1U); tooLong[sizeof(tooLong)-1U] = '\0';
    CHECK(umi_data_server_set(server, key, tooLong) == UMI_STATUS_OK);
    CHECK(UmiBuildProfileStoreLoad(server, "Umicom Notes", &loaded, &revision) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&before, &loaded, sizeof(loaded)) == 0);
    umi_data_server_destroy(server);
    /* An orphan field without the schema is corrupt, not permission to recreate. */
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    CHECK(umi_data_server_set(server, key, "retained") == UMI_STATUS_OK);
    Profile(&original, "Umicom Notes"); revision = 90U;
    CHECK(UmiBuildProfileStoreLoad(server, "Umicom Notes", &loaded, &revision) == UMI_STATUS_PARSE_ERROR);
    CHECK(revision == 90U);
    CHECK(UmiBuildProfileStoreSave(server, &original, 0U, &revision) == UMI_STATUS_PARSE_ERROR);
    umi_data_server_destroy(server);
    return EXIT_SUCCESS;
}

static int Atomicity(void)
{
    UmiDataServer *server = NULL;
    UmiBuildProfile profile, loaded;
    uint64_t revision = 19U;
    char key[192], buffer[32];
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    Profile(&profile, "Umicom Notes");
    CHECK(umi_data_server_begin(server) == UMI_STATUS_OK);
    CHECK(UmiBuildProfileStoreSave(server, &profile, 0U, &revision) == UMI_STATUS_BUSY);
    CHECK(umi_data_server_set(server, "caller.transaction", "not committed") == UMI_STATUS_OK);
    CHECK(umi_data_server_rollback(server) == UMI_STATUS_OK);
    CHECK(umi_data_server_get(server, "caller.transaction", buffer, sizeof(buffer)) == UMI_STATUS_NOT_FOUND);
    /* Leave fewer slots than one profile requires. A failed write must remove
     * all of its fields while preserving unrelated values. */
    for (unsigned i = 0U; i < 2040U; ++i) {
        (void)snprintf(key, sizeof(key), "unrelated.%u", i);
        CHECK(umi_data_server_set(server, key, "retained") == UMI_STATUS_OK);
    }
    CHECK(UmiBuildProfileStoreSave(server, &profile, 0U, &revision) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(revision == 19U);
    CHECK(umi_data_server_get(server, "unrelated.2039", buffer, sizeof(buffer)) == UMI_STATUS_OK);
    CHECK(strcmp(buffer, "retained") == 0);
    CHECK(UmiBuildProfileStoreLoad(server, "Umicom Notes", &loaded, &revision) == UMI_STATUS_NOT_FOUND);
    CHECK(revision == 0U);
    umi_data_server_destroy(server);
    return EXIT_SUCCESS;
}

static int Durable(void)
{
#ifndef UMICOM_HAS_SQLITE
    return 77; /* Missing backend is a reported skip, never a passing durability check. */
#else
    UmiDataServer *first = NULL, *second = NULL;
    UmiBuildProfile original, loaded;
    UmiClock clock = umi_clock_system();
    uint64_t revision = 0U;
    char database[256], trustKey[192], trustValue[64];
    CHECK(snprintf(database, sizeof(database), "profile-%" PRIu64 ".sqlite3",
        clock.wall_nanoseconds(&clock)) > 0);
    CHECK(!umi_fs_exists(database));
    CHECK(umi_data_server_create_sqlite(database, &first) == UMI_STATUS_OK);
    Profile(&original, "Umicom Notes");
    CHECK(UmiBuildProfileStoreSave(first, &original, 0U, &revision) == UMI_STATUS_OK);
    umi_data_server_destroy(first); first = NULL;
    CHECK(umi_data_server_create_sqlite(database, &first) == UMI_STATUS_OK);
    CHECK(umi_data_server_create_sqlite(database, &second) == UMI_STATUS_OK);
    CHECK(UmiBuildProfileStoreLoad(first, "Umicom Notes", &loaded, &revision) == UMI_STATUS_OK && revision == 1U);
    CHECK(strcmp(loaded.run_program, original.run_program) == 0);
    original.parallel_jobs = 3U;
    CHECK(UmiBuildProfileStoreSave(second, &original, 1U, &revision) == UMI_STATUS_OK && revision == 2U);
    loaded.parallel_jobs = 4U;
    CHECK(UmiBuildProfileStoreSave(first, &loaded, 1U, &revision) == UMI_STATUS_INVALID_STATE);
    CHECK(revision == 2U);
    CHECK(UmiBuildProfileStoreLoad(first, "Umicom Notes", &loaded, &revision) == UMI_STATUS_OK);
    CHECK(loaded.parallel_jobs == 3U && revision == 2U);
    CHECK(Key("Umicom Notes", "trust", trustKey) == EXIT_SUCCESS);
    CHECK(umi_data_server_get(first, trustKey, trustValue, sizeof(trustValue)) == UMI_STATUS_NOT_FOUND);
    umi_data_server_destroy(first); umi_data_server_destroy(second);
    /* Remove only the unique database created by this test. */
    CHECK(umi_fs_remove_tree(database) == UMI_STATUS_OK);
    return EXIT_SUCCESS;
#endif
}

int main(int argc, char **argv)
{
    if (argc != 2) return EXIT_FAILURE;
    if (strcmp(argv[1], "roundtrip") == 0) return RoundTrip();
    if (strcmp(argv[1], "corrupt") == 0) return Corrupt();
    if (strcmp(argv[1], "boundaries") == 0) return Boundaries();
    if (strcmp(argv[1], "atomicity") == 0) return Atomicity();
    if (strcmp(argv[1], "durable") == 0) return Durable();
    return EXIT_FAILURE;
}
