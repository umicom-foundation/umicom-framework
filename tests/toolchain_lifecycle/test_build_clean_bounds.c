/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_build_clean_bounds.c
 * PURPOSE: Check source protection and CMake cache ownership for explicitly requested cleanup.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/toolchain/build_execution.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/clock.h"
#include <stdio.h>
#include <stdlib.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); goto finish; } } while (0)
int main(void)
{
    int passed = 0, owns = 0;
    char temp[UMI_PATH_CAPACITY], project[UMI_PATH_CAPACITY] = {0}, binary[UMI_PATH_CAPACITY], cache[UMI_PATH_CAPACITY];
    char contents[UMI_PATH_CAPACITY + 64U], leaf[128];
    UmiBuildReport *report = calloc(1U, sizeof *report);
    CHECK(report);
    UmiClock clock = umi_clock_system();
    (void)snprintf(leaf, sizeof leaf, "umicom-clean-%llu", (unsigned long long)clock.monotonic_nanoseconds(&clock));
    CHECK(umi_fs_temp_directory(temp, sizeof temp) == UMI_STATUS_OK);
    CHECK(umi_path_join(temp, leaf, project, sizeof project) == UMI_STATUS_OK);
    CHECK(!umi_fs_exists(project));
    CHECK(umi_fs_make_directories(project) == UMI_STATUS_OK); owns = 1;
    CHECK(umi_path_join(project, "build", binary, sizeof binary) == UMI_STATUS_OK);
    CHECK(umi_fs_make_directories(binary) == UMI_STATUS_OK);
    CHECK(umi_path_join(binary, "CMakeCache.txt", cache, sizeof cache) == UMI_STATUS_OK);
    UmiToolchainProfile profile; umi_toolchain_profile_init(&profile);
    UmiBuildRequest request; umi_build_request_init(&request);
    request.source_root = project; request.build_directory = project;
    CHECK(umi_build_execute(&profile, NULL, UMI_BUILD_CLEAN, &request, report) != UMI_STATUS_OK);
    CHECK(umi_fs_is_directory(project));
    request.build_directory = temp;
    CHECK(umi_build_execute(&profile, NULL, UMI_BUILD_CLEAN, &request, report) != UMI_STATUS_OK);
    request.build_directory = binary;
    CHECK(umi_build_execute(&profile, NULL, UMI_BUILD_CLEAN, &request, report) != UMI_STATUS_OK);
    CHECK(umi_fs_is_directory(binary));
    CHECK(umi_fs_write_text(cache, "CMAKE_HOME_DIRECTORY:INTERNAL=unrelated\n") == UMI_STATUS_OK);
    CHECK(umi_build_execute(&profile, NULL, UMI_BUILD_CLEAN, &request, report) != UMI_STATUS_OK);
    (void)snprintf(contents, sizeof contents, "CMAKE_HOME_DIRECTORY:INTERNAL=%s\n", project);
    CHECK(umi_fs_write_text(cache, contents) == UMI_STATUS_OK);
    UmiBuildExecutionOptions options = {0}; options.preview = 1;
    CHECK(UmiBuildExecuteWithOptions(&profile, NULL, UMI_BUILD_CLEAN, &request, &options, report) == UMI_STATUS_OK);
    CHECK(umi_fs_exists(cache));
    char log_path[UMI_PATH_CAPACITY];
    CHECK(umi_path_join(binary, "clean.log", log_path, sizeof log_path) == UMI_STATUS_OK);
    UmiBuildExecutionLog log;
    CHECK(UmiBuildExecuteLogged(&profile, NULL, UMI_BUILD_CLEAN, &request, NULL, log_path, &log, report) != UMI_STATUS_OK);
    CHECK(!log.created && umi_fs_exists(cache));
    CHECK(umi_build_execute(&profile, NULL, UMI_BUILD_CLEAN, &request, report) == UMI_STATUS_OK);
    CHECK(!umi_fs_exists(binary) && umi_fs_is_directory(project));
    passed = 1;
finish:
    free(report);
    if (owns && umi_fs_remove_tree(project) != UMI_STATUS_OK) passed = 0;
    return passed ? 0 : 1;
}
