/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_automatic_session_plan.c
 *
 * PURPOSE:
 *   Verify automatic project requests execute each admitted build stage once.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "umicom/build/project_session.h"
#include "umicom/platform/threading.h"
#include "umicom/platform/filesystem.h"

/* Real CMake/CTest stages record their execution independently of the session.
 * No compiler, display, broker or network connection is needed. */
static const char project[] =
    "cmake_minimum_required(VERSION 3.24)\n"
    "project(WorkflowFixture LANGUAGES NONE)\n"
    "file(APPEND \"${CMAKE_BINARY_DIR}/events.txt\" \"C\")\n"
    "file(MAKE_DIRECTORY \"${CMAKE_BINARY_DIR}/umicom\")\n"
    "configure_file(driver.cmake \"${CMAKE_BINARY_DIR}/umicom/automatic-tests-Debug.cmake\" COPYONLY)\n"
    "add_custom_target(stage ALL COMMAND \"${CMAKE_COMMAND}\" "
    "\"-DROOT=${CMAKE_BINARY_DIR}\" -P \"${CMAKE_SOURCE_DIR}/build-stage.cmake\" VERBATIM)\n"
    "if(BUILD_TESTING)\n"
    "  enable_testing()\n"
    "  add_test(NAME smoke COMMAND \"${CMAKE_COMMAND}\" "
    "\"-DROOT=${CMAKE_BINARY_DIR}\" -P \"${CMAKE_SOURCE_DIR}/test-stage.cmake\")\n"
    "endif()\n"
    "message(STATUS \"Fixture strict warnings: ${UMICOM_ENABLE_STRICT_WARNINGS}\")\n";
static const char driver[] =
    "cmake_minimum_required(VERSION 3.24)\n"
    "get_filename_component(ROOT \"${CMAKE_CURRENT_LIST_DIR}\" DIRECTORY)\n"
    "file(APPEND \"${ROOT}/events.txt\" \"D\")\n"
    "if(EXISTS \"${ROOT}/reject-driver\")\n"
    "  message(FATAL_ERROR \"Intentional driver rejection\")\n"
    "endif()\n";
static const char buildStage[] =
    "cmake_minimum_required(VERSION 3.24)\n"
    "file(APPEND \"${ROOT}/events.txt\" \"B\")\n"
    "if(EXISTS \"${ROOT}/reject-build\")\n"
    "  message(FATAL_ERROR \"Intentional build rejection\")\n"
    "endif()\n";
static const char testStage[] =
    "cmake_minimum_required(VERSION 3.24)\n"
    "file(APPEND \"${ROOT}/events.txt\" \"T\")\n"
    "if(EXISTS \"${ROOT}/reject-test\")\n"
    "  message(FATAL_ERROR \"Intentional test rejection\")\n"
    "endif()\n";

static void Put(const char *root, const char *name, const char *text)
{
    char file[UMI_PATH_CAPACITY];
    FixturePath(file, root, name);
    CHECK(umi_fs_write_text(file, text) == UMI_STATUS_OK);
}
static void Events(const char *root, const char *expected)
{
    char file[UMI_PATH_CAPACITY];
    FixturePath(file, root, "events.txt");
    size_t length;
    unsigned char *bytes = FixtureRead(file, &length);
    CHECK(length == strlen(expected));
    CHECK(memcmp(bytes, expected, length) == 0);
    free(bytes);
}
static int Wait(UmiBuildProjectSession *session, UmiBuildProjectSessionSnapshot *state)
{
    for (unsigned attempt = 0U; attempt < 30000U; ++attempt) {
        if (umi_build_project_session_snapshot(session, state) != UMI_STATUS_OK) return 0;
        if (!state->active) return 1;
        umi_thread_sleep_ms(2U);
    }
    return 0;
}
static void Completed(UmiBuildProjectSession *session, const char *root,
    const char *events, size_t phases, UmiBuildPhase last, int success)
{
    UmiBuildProjectSessionSnapshot state;
    CHECK(Wait(session, &state));
    CHECK(!state.active && !state.cancellation_requested);
    CHECK(state.completed_phase_count == phases);
    CHECK(state.current_phase == last);
    CHECK((state.status == UMI_STATUS_OK) == success);
    UmiBuildResult *result = NULL;
    CHECK(umi_build_result_create(&result) == UMI_STATUS_OK);
    CHECK(umi_build_project_session_result_at(session, phases - 1U, result) == UMI_STATUS_OK);
    CHECK(result->phase == last);
    CHECK((result->status == UMI_STATUS_OK && result->exit_code == 0) == success);
    if (!success) CHECK(strstr(result->output, "Intentional") != NULL);
    umi_build_result_destroy(result);
    Events(root, events);
}
/* Completion is published before the queue retires its worker slot. A new
 * request may briefly be busy even after its result is readable. */
static UmiStatus Submit(UmiBuildProjectSession *session, const UmiBuildProfile *profile,
    UmiBuildPhase phase)
{
    for (unsigned attempt = 0U; attempt < 5000U; ++attempt) {
        UmiStatus status = UmiBuildProjectSessionSubmitAutomatic(session, profile, phase, true, NULL);
        if (status != UMI_STATUS_BUSY) return status;
        umi_thread_sleep_ms(1U);
    }
    return UMI_STATUS_BUSY;
}

int main(int argc, char **argv)
{
    if (argc != 5) return 2;
    const char *mode = argv[1];
    const char *names[] = {"bootstrap-build", "bootstrap-test", "reject-test",
        "reject-build", "existing-driver", "reject-driver", "disabled", "repeat"};
    int known = 0;
    for (size_t index = 0U; index < sizeof(names) / sizeof(names[0]); ++index)
        if (strcmp(mode, names[index]) == 0) known = 1;
    if (!known) return 2;
    char root[UMI_PATH_CAPACITY], source[UMI_PATH_CAPACITY], build[UMI_PATH_CAPACITY];
    char directory[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(source, root, "project with spaces");
    FixturePath(build, source, "build");
    FixturePath(directory, build, "umicom");
    CHECK(umi_fs_make_directories(directory) == UMI_STATUS_OK);
    Put(source, "CMakeLists.txt", project);
    Put(source, "build-stage.cmake", buildStage);
    Put(source, "test-stage.cmake", testStage);
    Put(source, "driver.cmake", driver);
    int existing = strcmp(mode, "existing-driver") == 0 || strcmp(mode, "reject-driver") == 0;
    int failure = strcmp(mode, "reject-build") == 0 || strcmp(mode, "reject-test") == 0 ||
        strcmp(mode, "reject-driver") == 0;
    int disabled = strcmp(mode, "disabled") == 0;
    if (existing) Put(directory, "automatic-tests-Debug.cmake", driver);
    if (failure) Put(build, mode, "Intentional fixture rejection.\n");
    UmiBuildProfile *profile = malloc(sizeof(*profile));
    CHECK(profile != NULL);
    CHECK(umi_build_profile_set(profile, "automatic-plan", source, "build") == UMI_STATUS_OK);
    CHECK(strlen(argv[2]) < sizeof(profile->tool_directory));
    CHECK(strlen(argv[3]) < sizeof(profile->generator));
    strcpy(profile->tool_directory, argv[2]);
    strcpy(profile->generator, argv[3]);
    /* Preserve the outer generator. Its build tool can be outside the test
     * process PATH, so pass the resolved tool as a project definition. */
    if (argv[4][0] != '\0') {
        int size = snprintf(profile->configure_definitions, sizeof(profile->configure_definitions),
            "\"-DCMAKE_MAKE_PROGRAM=%s\"", argv[4]);
        CHECK(size > 0 && (size_t)size < sizeof(profile->configure_definitions));
    }
    profile->build_testing = !disabled;
    profile->timeout_ms = 30000U;
    UmiBuildProjectSessionConfig config = {0};
    UmiBuildProjectSession *session = NULL;
    CHECK(umi_build_project_session_create(&config, &session) == UMI_STATUS_OK);
    UmiBuildPhase requested = strcmp(mode, "bootstrap-test") == 0
        ? UMI_BUILD_PHASE_TEST : UMI_BUILD_PHASE_BUILD;
    CHECK(Submit(session, profile, requested) == UMI_STATUS_OK);
    Completed(session, build,
        existing ? "D" : disabled || strcmp(mode, "reject-build") == 0 ? "CB" : "CBT",
        existing ? 1U : disabled || strcmp(mode, "reject-build") == 0 ? 2U : 3U,
        disabled || strcmp(mode, "reject-build") == 0 ? UMI_BUILD_PHASE_BUILD : UMI_BUILD_PHASE_TEST,
        !failure);
    if (strcmp(mode, "repeat") == 0) {
        /* A newly generated driver is eligible on the next request. A later
         * plain build must retire the preceding automatic selection. */
        CHECK(Submit(session, profile, UMI_BUILD_PHASE_BUILD) == UMI_STATUS_OK);
        Completed(session, build, "CBTD", 1U, UMI_BUILD_PHASE_TEST, 1);
        profile->build_testing = false;
        CHECK(Submit(session, profile, UMI_BUILD_PHASE_BUILD) == UMI_STATUS_OK);
        Completed(session, build, "CBTDCB", 2U, UMI_BUILD_PHASE_BUILD, 1);
    }
    umi_build_project_session_destroy(session);
    free(profile);
    return 0;
}
