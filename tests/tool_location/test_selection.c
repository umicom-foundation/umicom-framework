/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/tool_location/test_selection.c
 * PURPOSE: Check explicit tool selection, profile identity and bounded child search paths.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/cmake_provider.h"
#include "umicom/build/cpack_provider.h"
#include "umicom/build/ctest_provider.h"
#include "umicom/build/job_history.h"
#include "umicom/build/ninja_provider.h"
#include "umicom/build/tool_location.h"
#include "umicom/platform/process_search_path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value)                                                                               \
    do                                                                                             \
    {                                                                                              \
        if (!(value))                                                                              \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                            \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
#ifdef _WIN32
#define TOOL_ROOT "C:\\Developer Tools\\caf\xc3\xa9\\bin"
#define PROJECT_ROOT "C:\\Project Files\\notes"
#define PATH_SUFFIX "C:\\Windows;C:\\Other"
#define JOINED_PATH TOOL_ROOT ";" PATH_SUFFIX
#else
#define TOOL_ROOT "/opt/developer tools/caf\xc3\xa9/bin"
#define PROJECT_ROOT "/tmp/project files/notes"
#define PATH_SUFFIX "/usr/bin:/other"
#define JOINED_PATH TOOL_ROOT ":" PATH_SUFFIX
#endif

static int Selection(void)
{
    UmiBuildProfile profile, before;
    CHECK(umi_build_profile_set(&profile, "notes", PROJECT_ROOT, "build") == UMI_STATUS_OK);
    before = profile;
    char out[UMI_BUILD_PATH_CAPACITY], expected[UMI_BUILD_PATH_CAPACITY];
    CHECK(UmiBuildToolProgram(&profile, "cmake", out, sizeof out) == UMI_STATUS_OK);
    CHECK(strcmp(out, "cmake") == 0);
    strcpy(profile.tool_directory, TOOL_ROOT);
    CHECK(!umi_build_profile_equal(&profile, &before));
    UmiJobIdentity inherited, selected;
    CHECK(UmiBuildProfileJobIdentity(&before, &inherited) == UMI_STATUS_OK);
    CHECK(UmiBuildProfileJobIdentity(&profile, &selected) == UMI_STATUS_OK);
    CHECK(strcmp(inherited.configuration, selected.configuration) != 0);
    const char *names[] = {"cmake", "ctest", "cpack", "ninja"};
    UmiBuildProvider providers[] = {umi_build_cmake_provider(), umi_build_ctest_provider(),
                                    UmiBuildCPackProvider(), umi_build_ninja_provider()};
    UmiBuildPhase phases[] = {UMI_BUILD_PHASE_CONFIGURE, UMI_BUILD_PHASE_TEST,
                              UMI_BUILD_PHASE_PACKAGE, UMI_BUILD_PHASE_BUILD};
    for (size_t index = 0; index < 4U; ++index)
    {
        UmiBuildCommand *command = calloc(1U, sizeof *command);
        CHECK(command != NULL);
        CHECK(UmiBuildToolProgram(&profile, names[index], expected, sizeof expected) ==
              UMI_STATUS_OK);
        CHECK(umi_build_provider_create_command(&providers[index], &profile, phases[index],
                                                command) == UMI_STATUS_OK);
        CHECK(strcmp(command->program, expected) == 0 && command->argument_count != 0U);
        CHECK(strstr(command->program, "Developer Tools") != NULL ||
              strstr(command->program, "developer tools") != NULL);
#ifdef _WIN32
        CHECK(strstr(command->program, ".exe") != NULL);
#endif
        free(command);
    }
    strcpy(out, "untouched");
    CHECK(UmiBuildToolProgram(&profile, "unsupported", out, sizeof out) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(strcmp(out, "untouched") == 0);
    CHECK(UmiBuildToolProgram(&profile, "cmake", out, 2U) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(strcmp(out, "untouched") == 0);
    return 0;
}

static int Validation(void)
{
    const char *invalid[] = {"", "relative", "../tools", "tools;other", NULL};
    for (size_t index = 0; index < sizeof invalid / sizeof invalid[0]; ++index)
        CHECK(UmiProcessSearchDirectoryValidate(invalid[index]) != UMI_STATUS_OK);
    CHECK(UmiProcessSearchDirectoryValidate(TOOL_ROOT) == UMI_STATUS_OK);
#ifdef _WIN32
    CHECK(UmiProcessSearchDirectoryValidate("C:\\tools;C:\\other") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiProcessSearchDirectoryValidate("C:tools") == UMI_STATUS_INVALID_ARGUMENT);
#else
    CHECK(UmiProcessSearchDirectoryValidate("/tools:/other") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiProcessSearchDirectoryValidate("/tools\\other") == UMI_STATUS_INVALID_ARGUMENT);
#endif
    CHECK(UmiProcessSearchDirectoryValidate(PROJECT_ROOT "\nother") == UMI_STATUS_INVALID_ARGUMENT);
    char out[1024] = "untouched";
    const char *names[] = {"", "../ctest", "ctest.exe", "folder/ctest", "tool name", "tool;other"};
    for (size_t index = 0; index < sizeof names / sizeof names[0]; ++index)
    {
        CHECK(UmiProcessToolProgram(TOOL_ROOT, names[index], out, sizeof out) != UMI_STATUS_OK);
        CHECK(strcmp(out, "untouched") == 0);
    }
    return 0;
}

static int SearchPath(void)
{
    char *out = NULL;
    CHECK(UmiProcessSearchPathJoin(TOOL_ROOT, PATH_SUFFIX, &out) == UMI_STATUS_OK);
    CHECK(strcmp(out, JOINED_PATH) == 0);
    UmiProcessSearchPathFree(out);
    CHECK(UmiProcessSearchPathJoin(TOOL_ROOT, "", &out) == UMI_STATUS_OK);
    CHECK(strcmp(out, TOOL_ROOT) == 0);
    UmiProcessSearchPathFree(out);
    CHECK(UmiProcessSearchPathJoin(TOOL_ROOT, NULL, &out) == UMI_STATUS_OK);
    CHECK(strcmp(out, TOOL_ROOT) == 0);
    UmiProcessSearchPathFree(out);
    out = (char *)"not owned";
    CHECK(UmiProcessSearchPathJoin("relative", PATH_SUFFIX, &out) == UMI_STATUS_INVALID_ARGUMENT &&
          out == NULL);
    CHECK(UmiProcessSearchPathJoin(TOOL_ROOT, PATH_SUFFIX, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    char *large = malloc(131073U);
    CHECK(large != NULL);
    memset(large, 'x', 131072U);
    large[131072U] = '\0';
    CHECK(UmiProcessSearchPathJoin(TOOL_ROOT, large, &out) == UMI_STATUS_CAPACITY_EXCEEDED &&
          out == NULL);
    free(large);
    UmiProcessSearchPathFree(NULL);
    return 0;
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    if (strcmp(argv[1], "selection") == 0)
        return Selection();
    if (strcmp(argv[1], "validation") == 0)
        return Validation();
    if (strcmp(argv[1], "search-path") == 0)
        return SearchPath();
    return 1;
}
