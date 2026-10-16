/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_build_invocation.c
 * PURPOSE: Reject ambiguous development requests before any process or filesystem mutation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/toolchain/build_invocation.h"
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
    CHECK(strstr(inv->binary, "project with spaces/output"));
    CHECK(strstr(inv->prefix, "project with spaces/installed"));
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
    CHECK(strstr(inv->executable, "custom/Debug/bin/umicom-studio-ide"));
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
    free(inv);
    return 0;
}
