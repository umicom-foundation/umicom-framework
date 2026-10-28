/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_verify.c
 *
 * PURPOSE:
 *   Build a configured project and let Framework select tests from retained evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "cli.h"
#include "umicom/build/ctest_provider.h"
#include "umicom/build/live_output.h"
#include "umicom/platform/output_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct VerifyOutput {
    UmiOutputFile *file;
    UmiStatus status;
} VerifyOutput;
static void VerifyBytes(const char *bytes, size_t size, void *context)
{
    VerifyOutput *output = context;
    if (fwrite(bytes, 1U, size, stdout) != size && output->status == UMI_STATUS_OK)
        output->status = UMI_STATUS_IO_ERROR;
    (void)fflush(stdout);
    if (output->status == UMI_STATUS_OK)
        output->status = UmiOutputFileWrite(output->file, bytes, size);
}
int umi_cli_command_verify(int argc, char **argv)
{
    const char *source = ".", *binary = NULL, *configuration = "Debug";
    unsigned seen = 0U;
    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "--help") == 0 && argc == 1) {
            puts("Usage: umicom verify --build BUILD_DIRECTORY [--source PROJECT] [--config Debug]\n"
                 "Uses the existing build cache; no target or test filter is required.\n"
                 "Configured Umicom projects reuse only unchanged closed-input passing tests.\n"
                 "Other projects retain ordinary full CTest. Logs stay inside the build directory.");
            return 0;
        }
        unsigned bit = strcmp(argv[i], "--source") == 0 ? 1U :
            strcmp(argv[i], "--build") == 0 ? 2U : strcmp(argv[i], "--config") == 0 ? 4U : 0U;
        if (bit == 0U || (seen & bit) != 0U || i + 1 >= argc || !argv[i + 1][0]) {
            fputs("Invalid verify options. Use umicom verify --help.\n", stderr); return 2;
        }
        seen |= bit;
        const char *value = argv[++i];
        if (bit == 1U) source = value;
        else if (bit == 2U) binary = value;
        else configuration = value;
    }
    if (binary == NULL) { fputs("Select the configured project build directory with --build.\n", stderr); return 2; }
    UmiBuildRunnerConfig *config = calloc(1U, sizeof(*config));
    UmiBuildResult *result = calloc(1U, sizeof(*result));
    if (config == NULL || result == NULL) { free(config); free(result); return 1; }
    UmiClock clock = umi_clock_system(); config->clock = &clock;
    UmiStatus status = umi_build_profile_set(&config->profile, "automatic", source, binary);
    if (status == UMI_STATUS_OK && strlen(configuration) >= sizeof(config->profile.configuration))
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK) strcpy(config->profile.configuration, configuration);
    config->profile.parallel_jobs = 2U;
    char absolute_source[UMI_BUILD_PATH_CAPACITY], build[UMI_BUILD_PATH_CAPACITY];
    char log_root[UMI_PATH_CAPACITY], log_path[UMI_PATH_CAPACITY];
    if (status == UMI_STATUS_OK)
        status = UmiBuildProfileSourceDirectory(&config->profile, absolute_source, sizeof(absolute_source));
    if (status == UMI_STATUS_OK)
        status = UmiBuildProfileBuildDirectory(&config->profile, absolute_source, build, sizeof(build));
#ifdef _WIN32
    /* A Windows executable cannot replace its own loaded image. Use an
     * installed CLI outside the project being rebuilt; never work around the
     * operating system's file ownership or terminate another application. */
    if (status == UMI_STATUS_OK) {
        char image[UMI_PATH_CAPACITY];
        status = umi_fs_executable_path(image, sizeof(image));
        if (status == UMI_STATUS_OK && umi_path_is_within(build, image)) {
            fputs("Run the installed umicom command outside this build directory; Windows cannot rebuild the running CLI executable.\n", stderr);
            status = UMI_STATUS_INVALID_STATE;
        }
    }
#endif
    if (status == UMI_STATUS_OK) status = umi_fs_join(log_root, sizeof(log_root), build, ".umicom/verification-logs");
    if (status == UMI_STATUS_OK) status = umi_fs_make_directories(log_root);
    if (status == UMI_STATUS_OK) {
        int written = snprintf(log_path, sizeof(log_path), "%s/verify-%llu.log", log_root,
            (unsigned long long)clock.wall_nanoseconds(&clock));
        if (written < 0 || (size_t)written >= sizeof(log_path)) status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    VerifyOutput output = {NULL, UMI_STATUS_OK};
    if (status == UMI_STATUS_OK) status = UmiOutputFileCreate(log_path, &output.file);
    char driver[UMI_PATH_CAPACITY] = "";
    bool generated_driver = false;
    /* The original existence probe is retained; shared discovery also respects
     * profile validation and the same preset rules used by Studio. */
#if 0
    if (status == UMI_STATUS_OK) {
        int size = snprintf(driver, sizeof(driver), "%s/umicom/automatic-tests-%s.cmake", build, configuration);
        if (size < 0 || (size_t)size >= sizeof(driver)) status = UMI_STATUS_CAPACITY_EXCEEDED;
        else generated_driver = umi_fs_is_file(driver) != 0;
    }
#endif
    if (status == UMI_STATUS_OK) {
        UmiStatus discovery = UmiBuildAutomaticTestDriver(&config->profile, driver, sizeof(driver));
        generated_driver = discovery == UMI_STATUS_OK;
        if (discovery != UMI_STATUS_OK && discovery != UMI_STATUS_NOT_FOUND) status = discovery;
    }
    UmiBuildRunner *runner = NULL;
    if (status == UMI_STATUS_OK) status = umi_build_runner_create(config, &runner);
    if (status == UMI_STATUS_OK) {
        printf("Verification log: %s\n", log_path);
        /* The generated driver owns configure/build/test. A generic project
         * has no such driver, so retain its ordinary incremental build first. */
        if (!generated_driver)
            status = UmiBuildRunnerRunObserved(runner, UMI_BUILD_PHASE_BUILD, VerifyBytes, &output, result);
    }
    if (status == UMI_STATUS_OK && output.status == UMI_STATUS_OK)
        status = UmiBuildRunnerRunAutomaticTests(runner, VerifyBytes, &output, result);
    if (output.file != NULL) {
        UmiStatus closed = UmiOutputFileClose(output.file);
        if (status == UMI_STATUS_OK) status = closed;
    }
    if (status == UMI_STATUS_OK) status = output.status;
    if (status != UMI_STATUS_OK) fprintf(stderr, "Automatic verification stopped: %s\n", umi_status_text(status));
    UmiOutputFileDestroy(output.file); umi_build_runner_destroy(runner);
    umi_clock_dispose(&clock); free(config); free(result);
    return status == UMI_STATUS_OK ? 0 : 1;
}
