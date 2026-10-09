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

/* Settings opened by older applications retain their literal field. Current
 * records must contain the explicit list field, even when it is empty. */
static int ArgumentStorage(void)
{
    UmiDataServer *server = NULL;
    UmiBuildProfile profile, loaded, before;
    uint64_t revision = 0U;
    char field[192], schema[192];
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    Profile(&profile, "Launch arguments");
    CHECK(UmiBuildProfileStoreSave(server, &profile, 0U, &revision) == UMI_STATUS_OK);
    CHECK(Key("Launch arguments", "run_arguments", field) == EXIT_SUCCESS);
    CHECK(Key("Launch arguments", "schema", schema) == EXIT_SUCCESS);
    CHECK(umi_data_server_delete(server, field) == UMI_STATUS_OK);
    memset(&loaded, 0x5a, sizeof(loaded)); before = loaded;
    CHECK(UmiBuildProfileStoreLoad(server, "Launch arguments", &loaded, &revision) == UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(&loaded, &before, sizeof(loaded)) == 0);
    CHECK(umi_data_server_set(server, schema, "1") == UMI_STATUS_OK);
    CHECK(UmiBuildProfileStoreLoad(server, "Launch arguments", &loaded, &revision) == UMI_STATUS_OK);
    CHECK(loaded.run_arguments[0] == '\0' && strcmp(loaded.run_argument, profile.run_argument) == 0);
    loaded.run_argument[0] = '\0';
    strcpy(loaded.run_arguments, "--file \"caf\xc3\xa9 notes.txt\" \"\"");
    CHECK(UmiBuildProfileStoreSave(server, &loaded, revision, &revision) == UMI_STATUS_OK);
    profile = loaded;
    CHECK(UmiBuildProfileStoreLoad(server, "Launch arguments", &loaded, &revision) == UMI_STATUS_OK);
    CHECK(umi_build_profile_equal(&profile, &loaded));
    before = loaded;
    CHECK(umi_data_server_set(server, field, "\"unfinished") == UMI_STATUS_OK);
    CHECK(UmiBuildProfileStoreLoad(server, "Launch arguments", &loaded, &revision) == UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(&loaded, &before, sizeof(loaded)) == 0);
    umi_data_server_destroy(server);
    return EXIT_SUCCESS;
}


/* Exercise schema migration and damage through the public Data Server. Older
 * settings may omit stages, but current settings may never lose a stage field
 * unnoticed. Output snapshots and revision counters remain unchanged on error. */
static int StageStorage(const char *mode)
{
    UmiDataServer *server = NULL;
    UmiBuildProfile profile, loaded, before;
    char key[192], schema[192];
    uint64_t revision = 0U;
    static const char *const stages[] = {"configure_preset", "build_preset", "test_preset", "run_working_directory"};
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    Profile(&profile, "Stage presets");
    profile.preset[0] = '\0';
    CHECK(UmiBuildProfileStoreSave(server, &profile, 0U, &revision) == UMI_STATUS_OK);
    CHECK(Key(profile.source_directory, "schema", schema) == EXIT_SUCCESS);
    if (strcmp(mode, "stage-migrate") == 0) {
        for (unsigned old = 1U; old <= 2U; ++old) {
            for (size_t i = 0U; i < sizeof(stages) / sizeof(stages[0]); ++i) {
                CHECK(Key(profile.source_directory, stages[i], key) == EXIT_SUCCESS);
                CHECK(umi_data_server_delete(server, key) == UMI_STATUS_OK);
            }
            CHECK(umi_data_server_set(server, schema, old == 1U ? "1" : "2") == UMI_STATUS_OK);
            CHECK(UmiBuildProfileStoreLoad(server, profile.source_directory, &loaded, &revision) == UMI_STATUS_OK);
            CHECK(loaded.configure_preset[0] == '\0' && loaded.build_preset[0] == '\0' && loaded.test_preset[0] == '\0');
            CHECK(UmiBuildProfileStoreSave(server, &loaded, revision, &revision) == UMI_STATUS_OK);
            char marker[8];
/* Migration now writes the required tool-folder field with the profile. The prior format expectation remains for storage compatibility review. The previous implementation is retained for engineering review. */
#if 0
            CHECK(umi_data_server_get(server, schema, marker, sizeof(marker)) == UMI_STATUS_OK && strcmp(marker, "3") == 0);
#endif
/* Migration now writes the required configure-definitions field. Keep the preceding stored-format expectation for review. The previous implementation is retained for engineering review. */
#if 0
            CHECK(umi_data_server_get(server, schema, marker, sizeof(marker)) == UMI_STATUS_OK && strcmp(marker, "4") == 0);
#endif
/* Migration now retains launch environment settings as required fields. The preceding format expectation remains for compatibility review. The previous implementation is retained for engineering review. */
#if 0
            CHECK(umi_data_server_get(server, schema, marker, sizeof(marker)) == UMI_STATUS_OK && strcmp(marker, "5") == 0);
#endif
            CHECK(umi_data_server_get(server, schema, marker, sizeof(marker)) == UMI_STATUS_OK && strcmp(marker, "6") == 0);
        }
    } else if (strcmp(mode, "stage-missing") == 0) {
        CHECK(UmiBuildProfileStoreLoad(server, profile.source_directory, &loaded, &revision) == UMI_STATUS_OK);
        before = loaded;
        for (size_t i = 0U; i < sizeof(stages) / sizeof(stages[0]); ++i) {
            CHECK(Key(profile.source_directory, stages[i], key) == EXIT_SUCCESS);
            CHECK(umi_data_server_delete(server, key) == UMI_STATUS_OK);
            CHECK(UmiBuildProfileStoreLoad(server, profile.source_directory, &loaded, &revision) == UMI_STATUS_PARSE_ERROR);
            CHECK(revision == 1U && memcmp(&loaded, &before, sizeof(loaded)) == 0);
            CHECK(UmiBuildProfileStoreSave(server, &profile, 1U, &revision) == UMI_STATUS_PARSE_ERROR);
            CHECK(umi_data_server_set(server, key, "") == UMI_STATUS_OK);
        }
    } else {
        strcpy(profile.configure_preset, "notes-configure");
        strcpy(profile.build_preset, "notes-build");
        strcpy(profile.test_preset, "notes-test");
        CHECK(UmiBuildProfileStoreSave(server, &profile, 1U, &revision) == UMI_STATUS_OK);
        CHECK(UmiBuildProfileStoreLoad(server, profile.source_directory, &loaded, &revision) == UMI_STATUS_OK);
        strcpy(profile.source_directory, loaded.source_directory);
        CHECK(umi_build_profile_equal(&profile, &loaded));
        before = loaded;
        if (strcmp(mode, "stage-downgrade") == 0) {
            CHECK(umi_data_server_set(server, schema, "2") == UMI_STATUS_OK);
            CHECK(UmiBuildProfileStoreLoad(server, profile.source_directory, &loaded, &revision) == UMI_STATUS_PARSE_ERROR);
            CHECK(revision == 2U && memcmp(&loaded, &before, sizeof(loaded)) == 0);
        } else {
            strcpy(profile.build_preset, "changed-by-other-editor");
            CHECK(UmiBuildProfileStoreSave(server, &profile, 1U, &revision) == UMI_STATUS_INVALID_STATE);
            CHECK(UmiBuildProfileStoreLoad(server, profile.source_directory, &loaded, &revision) == UMI_STATUS_OK);
            CHECK(umi_build_profile_equal(&before, &loaded) && revision == 2U);
        }
    }
    umi_data_server_destroy(server);
    return EXIT_SUCCESS;
}


/* A tool change is a settings change, not a global environment mutation.
 * Exercise migration and corruption through the public database contract
 * used when Studio reopens a project. */
static int ToolStorage(const char *mode)
{
    UmiDataServer *server = NULL;
    UmiBuildProfile profile, loaded, before;
    uint64_t revision = 0U;
    char field[192], schema[192];
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    Profile(&profile, "Tool selection");
    CHECK(UmiBuildProfileStoreSave(server,&profile,0U,&revision) == UMI_STATUS_OK);
    CHECK(Key(profile.source_directory,"tool_directory",field) == EXIT_SUCCESS);
    CHECK(Key(profile.source_directory,"schema",schema) == EXIT_SUCCESS);
    CHECK(UmiBuildProfileStoreLoad(server,profile.source_directory,&loaded,&revision) == UMI_STATUS_OK);
    before = loaded;
    if (strcmp(mode,"tool-migrate") == 0) {
        CHECK(umi_data_server_delete(server,field) == UMI_STATUS_OK);
        CHECK(umi_data_server_set(server,schema,"3") == UMI_STATUS_OK);
        CHECK(UmiBuildProfileStoreLoad(server,profile.source_directory,&loaded,&revision) == UMI_STATUS_OK);
        CHECK(loaded.tool_directory[0] == '\0');
        CHECK(UmiBuildProfileStoreSave(server,&loaded,revision,&revision) == UMI_STATUS_OK);
        char value[8];
        CHECK(umi_data_server_get(server,field,value,sizeof value) == UMI_STATUS_OK && value[0] == '\0');
    } else if (strcmp(mode,"tool-missing") == 0) {
        CHECK(umi_data_server_delete(server,field) == UMI_STATUS_OK);
        CHECK(UmiBuildProfileStoreLoad(server,profile.source_directory,&loaded,&revision) == UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(&loaded,&before,sizeof loaded) == 0);
    } else if (strcmp(mode,"tool-invalid") == 0) {
        CHECK(umi_data_server_set(server,field,"relative tools") == UMI_STATUS_OK);
        CHECK(UmiBuildProfileStoreLoad(server,profile.source_directory,&loaded,&revision) == UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(&loaded,&before,sizeof loaded) == 0);
    } else {
#ifdef _WIN32
        strcpy(profile.tool_directory,"C:\\Developer Tools\\bin");
#else
        strcpy(profile.tool_directory,"/opt/developer tools/bin");
#endif
        CHECK(UmiBuildProfileStoreSave(server,&profile,revision,&revision) == UMI_STATUS_OK);
        CHECK(UmiBuildProfileStoreLoad(server,profile.source_directory,&loaded,&revision) == UMI_STATUS_OK);
        CHECK(strcmp(loaded.tool_directory,profile.tool_directory) == 0);
        before=loaded;
        if (strcmp(mode,"tool-downgrade") == 0) {
            CHECK(umi_data_server_set(server,schema,"3") == UMI_STATUS_OK);
            CHECK(UmiBuildProfileStoreLoad(server,profile.source_directory,&loaded,&revision) == UMI_STATUS_PARSE_ERROR);
            CHECK(memcmp(&loaded,&before,sizeof loaded) == 0);
        } else {
            profile.tool_directory[0]='\0';
            CHECK(!umi_build_profile_equal(&profile,&loaded));
            CHECK(UmiBuildProfileStoreSave(server,&profile,revision,&revision) == UMI_STATUS_OK);
            CHECK(UmiBuildProfileStoreLoad(server,profile.source_directory,&loaded,&revision) == UMI_STATUS_OK);
            CHECK(loaded.tool_directory[0]=='\0');
        }
    }
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
    /* Exercise the list field through two real SQLite connections as well as
     * the memory-backend migration checks. Reopening must retain empty values
     * and spaces, without bringing back the superseded literal input. */
    original.run_argument[0] = '\0';
    strcpy(original.run_arguments, "--file \"saved notes.txt\" \"\"");
    original.parallel_jobs = 3U;
    /* Reopen independent stages through SQLite as well as the memory store. */
    original.preset[0] = '\0';
    strcpy(original.configure_preset, "sqlite-configure");
    strcpy(original.build_preset, "sqlite-build");
    strcpy(original.test_preset, "sqlite-test");
    strcpy(original.run_working_directory, "data files");
#ifdef _WIN32
    strcpy(original.tool_directory,"C:\\Developer Tools\\bin");
#else
    strcpy(original.tool_directory,"/opt/developer tools/bin");
#endif
    CHECK(UmiBuildProfileStoreSave(second, &original, 1U, &revision) == UMI_STATUS_OK && revision == 2U);
    loaded.parallel_jobs = 4U;
    CHECK(UmiBuildProfileStoreSave(first, &loaded, 1U, &revision) == UMI_STATUS_INVALID_STATE);
    CHECK(revision == 2U);
    CHECK(UmiBuildProfileStoreLoad(first, "Umicom Notes", &loaded, &revision) == UMI_STATUS_OK);
    CHECK(loaded.parallel_jobs == 3U && revision == 2U);
    CHECK(strcmp(loaded.run_arguments, original.run_arguments) == 0 && loaded.run_argument[0] == '\0');
    umi_data_server_destroy(first); first = NULL;
    CHECK(umi_data_server_create_sqlite(database, &first) == UMI_STATUS_OK);
    CHECK(UmiBuildProfileStoreLoad(first, "Umicom Notes", &loaded, &revision) == UMI_STATUS_OK);
    CHECK(strcmp(loaded.run_arguments, original.run_arguments) == 0 && loaded.run_argument[0] == '\0');
    CHECK(strcmp(loaded.configure_preset, original.configure_preset) == 0);
    CHECK(strcmp(loaded.build_preset, original.build_preset) == 0);
    CHECK(strcmp(loaded.test_preset, original.test_preset) == 0);
    CHECK(strcmp(loaded.run_working_directory, original.run_working_directory) == 0);
    CHECK(strcmp(loaded.tool_directory, original.tool_directory) == 0);
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
    if (strncmp(argv[1], "tool-", 5U) == 0) return ToolStorage(argv[1]);
    if (strcmp(argv[1], "stage-migrate") == 0 || strcmp(argv[1], "stage-missing") == 0 ||
        strcmp(argv[1], "stage-downgrade") == 0 || strcmp(argv[1], "stage-roundtrip") == 0)
        return StageStorage(argv[1]);
    if (strcmp(argv[1], "arguments") == 0) return ArgumentStorage();
    if (strcmp(argv[1], "roundtrip") == 0) return RoundTrip();
    if (strcmp(argv[1], "corrupt") == 0) return Corrupt();
    if (strcmp(argv[1], "boundaries") == 0) return Boundaries();
    if (strcmp(argv[1], "atomicity") == 0) return Atomicity();
    if (strcmp(argv[1], "durable") == 0) return Durable();
    return EXIT_FAILURE;
}
