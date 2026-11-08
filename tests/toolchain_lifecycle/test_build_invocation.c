/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_build_invocation.c
 *
 * PURPOSE:
 *   Reject ambiguous development requests before any process or filesystem mutation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/toolchain/build_invocation.h"
#include "umicom/platform/path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)
#ifdef _WIN32
#define WORK "C:/work/project"
#else
#define WORK "/work/project"
#endif

int main(void)
{
    UmiBuildInvocation *inv = calloc(1U, sizeof *inv);
    CHECK(inv);
    char message[256];
    const char *valid[] = {"--source", "project with spaces", "--preset", "release",
        "--build", "output", "--prefix", "installed", "--define", "BUILD_TESTING=ON",
        "--jobs", "2", "--config", "Debug", "--log", "logs/run.txt", "--dry-run"};
    CHECK(UmiBuildInvocationParse(UMI_BUILD_DELIVER, 17, valid, WORK, inv, message, sizeof message) == UMI_STATUS_OK);
    CHECK(inv->options.preview && inv->options.explicit_build_directory);
    /* Superseded: slash-sensitive substring assertions assumed POSIX paths.
     * Windows Framework path normalisation uses native backslashes. Keep the
     * previous expectations below, disabled, for comparison. */
#if 0
    CHECK(strstr(inv->binary, "project with spaces/output"));
    CHECK(strstr(inv->prefix, "project with spaces/installed"));
#endif
    char expected_binary[UMI_PATH_CAPACITY], expected_prefix[UMI_PATH_CAPACITY];
    CHECK(umi_path_absolute("output", inv->source, expected_binary, sizeof expected_binary) == UMI_STATUS_OK);
    CHECK(umi_path_absolute("installed", inv->source, expected_prefix, sizeof expected_prefix) == UMI_STATUS_OK);
    CHECK(umi_path_equal(inv->binary, expected_binary));
    CHECK(umi_path_equal(inv->prefix, expected_prefix));
    CHECK(inv->request.jobs == 2 && inv->options.configure_definition_count == 1U);
    CHECK(strcmp(inv->definitions[0], "BUILD_TESTING=ON") == 0);
    const char *bad[][4] = {
        {"--jobs", "2oops", NULL, NULL}, {"--jobs", "-1", NULL, NULL},
        {"--jobs", "0", NULL, NULL}, {"--jobs", "999999999999999999999", NULL, NULL},
        {"--preset", NULL, NULL, NULL}, {"--log", "", NULL, NULL},
        {"--jobs", "1", "--jobs", "2"}, {"--typo", NULL, NULL, NULL},
        {"--prefix", "elsewhere", NULL, NULL}, {"--define", "X=Y", NULL, NULL},
        {"--timeout-ms", "-1", NULL, NULL}, {"--", "--verbose", NULL, NULL}
    };
    for (size_t i = 0U; i < sizeof bad / sizeof bad[0]; ++i) {
        int count = 0; while (count < 4 && bad[i][count]) ++count;
        CHECK(UmiBuildInvocationParse(UMI_BUILD_COMPILE, count, bad[i], WORK, inv, message, sizeof message) != UMI_STATUS_OK);
    }
    const char *install[] = {"--preset", "release"};
    CHECK(UmiBuildInvocationParse(UMI_BUILD_INSTALL, 2, install, WORK, inv, message, sizeof message) != UMI_STATUS_OK);
    const char *launch[] = {"--executable", "bin/app", "--", "", "two words", "$(literal)", "--log"};
    CHECK(UmiBuildInvocationParse(UMI_BUILD_RUN, 7, launch, WORK, inv, message, sizeof message) == UMI_STATUS_OK);
    CHECK(inv->request.run_argument_count == 4U && strcmp(inv->request.run_arguments[2], "$(literal)") == 0);
    CHECK(inv->log[0] == '\0');
    const char *alias[] = {"studio", "--build", "custom/Debug"};
    CHECK(UmiBuildInvocationParse(UMI_BUILD_RUN, 3, alias, WORK, inv, message, sizeof message) == UMI_STATUS_OK);
    /* Superseded: executable substring assumed / on Windows and omitted
     * the platform-dependent executable suffix. Keep it for code review. */
#if 0
    CHECK(strstr(inv->executable, "custom/Debug/bin/umicom-studio-ide"));
#endif
    char expected_executable[UMI_PATH_CAPACITY];
#ifdef _WIN32
    const char *expected_name = "bin/umicom-studio-ide.exe";
#else
    const char *expected_name = "bin/umicom-studio-ide";
#endif
    CHECK(umi_path_absolute(expected_name, inv->binary, expected_executable, sizeof expected_executable) == UMI_STATUS_OK);
    CHECK(umi_path_equal(inv->executable, expected_executable));
    CHECK(UmiBuildInvocationParse(UMI_BUILD_RUN, 1, alias, WORK, inv, message, sizeof message) != UMI_STATUS_OK);
    const char *escape[] = {"../unrelated", "--build", "output"};
    CHECK(UmiBuildInvocationParse(UMI_BUILD_RUN, 3, escape, WORK, inv, message, sizeof message) != UMI_STATUS_OK);
    const char *definitions[34];
    for (size_t i = 0U; i < 17U; ++i) { definitions[i * 2U] = "--define"; definitions[i * 2U + 1U] = "X=Y"; }
    CHECK(UmiBuildInvocationParse(UMI_BUILD_CONFIGURE, 34, definitions, WORK, inv, message, sizeof message) != UMI_STATUS_OK);
    const char *jobs[] = {"--jobs", "1024"};
    CHECK(UmiBuildInvocationParse(UMI_BUILD_TEST, 2, jobs, WORK, inv, message, sizeof message) == UMI_STATUS_OK);
    CHECK(inv->request.jobs == 1024);
    const char *empty[] = {NULL};
    CHECK(UmiBuildInvocationParse(UMI_BUILD_COMPILE, 1, empty, WORK, inv, message, sizeof message) != UMI_STATUS_OK);
    /* Reproduce the owner's CTest artifact paths and Ninja keep-going values
     * through the public parser. No tool, filesystem or shell is invoked. */
    const char *artifacts[] = {"--source", "project with spaces", "--build", "output",
        "--output-log", "logs/test output.log", "--output-junit", "logs/results.xml"};
    CHECK(UmiBuildInvocationParse(UMI_BUILD_TEST, 8, artifacts, WORK, inv, message, sizeof message) == UMI_STATUS_OK);
    char expected_artifact[UMI_PATH_CAPACITY];
    CHECK(umi_path_absolute("logs/test output.log", inv->source, expected_artifact, sizeof expected_artifact) == UMI_STATUS_OK);
    CHECK(umi_path_equal(inv->options.test_output_log, expected_artifact));
    CHECK(inv->options.test_output_junit == inv->test_junit);
    CHECK(UmiBuildInvocationParse(UMI_BUILD_COMPILE, 8, artifacts, WORK, inv, message, sizeof message) == UMI_STATUS_INVALID_ARGUMENT);
    const char *native[] = {"--build", "output", "--build-argument", "-k", "--build-argument", "0",
        "--build-argument", "NAME=two words;$(literal)"};
    CHECK(UmiBuildInvocationParse(UMI_BUILD_COMPILE, 8, native, WORK, inv, message, sizeof message) == UMI_STATUS_OK);
    CHECK(inv->options.build_argument_count == 3U);
    CHECK(strcmp(inv->options.build_arguments[0], "-k") == 0);
    CHECK(strcmp(inv->options.build_arguments[1], "0") == 0);
    CHECK(strcmp(inv->options.build_arguments[2], "NAME=two words;$(literal)") == 0);
    CHECK(UmiBuildInvocationParse(UMI_BUILD_TEST, 8, native, WORK, inv, message, sizeof message) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiBuildInvocationParse(UMI_BUILD_CONFIGURE, 8, native, WORK, inv, message, sizeof message) == UMI_STATUS_INVALID_ARGUMENT);
    const char *artifact_bad[][4] = {
        {"--output-log", "one.log", "--output-log", "two.log"},
        {"--output-junit", "", NULL, NULL},
        {"--output-log", "same.log", "--output-junit", "./same.log"},
        {"--log", "same.log", "--output-junit", "same.log"},
        {"--log", "same.log", "--output-log", "same.log"}
    };
    for (size_t i = 0U; i < sizeof artifact_bad / sizeof artifact_bad[0]; ++i) {
        int count = 0; while (count < 4 && artifact_bad[i][count]) ++count;
        CHECK(UmiBuildInvocationParse(UMI_BUILD_TEST, count, artifact_bad[i], WORK, inv, message, sizeof message) == UMI_STATUS_INVALID_ARGUMENT);
    }
    const char *missing_native[] = {"--build-argument"};
    CHECK(UmiBuildInvocationParse(UMI_BUILD_COMPILE, 1, missing_native, WORK, inv, message, sizeof message) == UMI_STATUS_INVALID_ARGUMENT);
    const char *empty_native[] = {"--build-argument", ""};
    CHECK(UmiBuildInvocationParse(UMI_BUILD_COMPILE, 2, empty_native, WORK, inv, message, sizeof message) == UMI_STATUS_INVALID_ARGUMENT);
    const char *many_native[34];
    for (size_t i = 0U; i < 17U; ++i) { many_native[i * 2U] = "--build-argument"; many_native[i * 2U + 1U] = "value"; }
    CHECK(UmiBuildInvocationParse(UMI_BUILD_COMPILE, 32, many_native, WORK, inv, message, sizeof message) == UMI_STATUS_OK);
    CHECK(inv->options.build_argument_count == 16U);
    CHECK(UmiBuildInvocationParse(UMI_BUILD_COMPILE, 34, many_native, WORK, inv, message, sizeof message) == UMI_STATUS_INVALID_ARGUMENT);
    free(inv);
    return 0;
}
