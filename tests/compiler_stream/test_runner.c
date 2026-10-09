/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_stream/test_runner.c
 * PURPOSE: Verify that the real process runner retains diagnostics beyond its preview bound.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/live_output.h"
#include "umicom/build/runner.h"
#include "umicom/platform/filesystem.h"
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
static void CountBytes(const char *bytes, size_t length, void *context)
{
    size_t *count = context;
    /* The observer still receives every byte; diagnostic parsing must not
     * consume or modify the output supplied to log capture and the console. */
    if (bytes != NULL)
        *count += length;
}
int main(int argc, char **argv)
{
    CHECK(argc == 3);
    UmiBuildRunnerConfig config = {0};
    UmiBuildRunner *runner = NULL;
    UmiBuildResult *result = calloc(1U, sizeof *result);
    size_t bytes = 0U;
    CHECK(result != NULL && umi_fs_is_absolute(argv[1]) && umi_fs_is_absolute(argv[2]));
    CHECK(umi_build_profile_set(&config.profile, "stream", argv[2], "build") == UMI_STATUS_OK);
    CHECK(strlen(argv[1]) < sizeof config.profile.run_program);
    strcpy(config.profile.run_program, argv[1]);
    strcpy(config.profile.run_argument, "emit");
    config.profile.timeout_ms = 10000U;
    CHECK(umi_build_runner_create(&config, &runner) == UMI_STATUS_OK);
    CHECK(UmiBuildRunnerRunObserved(runner, UMI_BUILD_PHASE_RUN, CountBytes, &bytes, result) ==
          UMI_STATUS_INTERNAL_ERROR);
    CHECK(result->exit_code == 7 && result->status == UMI_STATUS_INTERNAL_ERROR);
    CHECK(bytes > UMI_BUILD_OUTPUT_CAPACITY && strlen(result->output) < UMI_BUILD_OUTPUT_CAPACITY);
    CHECK(result->diagnostics.count == 3U && result->diagnostics.dropped == 0U);
    CHECK(strstr(result->diagnostics.items[0].file, "first.c") != NULL);
    CHECK(result->diagnostics.items[0].line == 2U && result->diagnostics.items[0].column == 3U);
    CHECK(strstr(result->diagnostics.items[1].message, "final configure explanation") != NULL);
    CHECK(strstr(result->diagnostics.items[2].file, "last.c") != NULL);
    umi_build_runner_destroy(runner);
    free(result);
    return 0;
}
