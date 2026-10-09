/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/run_environment/test_child.c
 * PURPOSE: Exercise actual Run and persistent child environment delivery without altering the parent.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/build/result.h"
#include "umicom/build/runner.h"
#include "umicom/language_runtime/process_stream.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/process_search_path.h"
int main(int argc, char **argv)
{
    CHECK(argc == 3);
    char *before = NULL, *after = NULL;
    CHECK(UmiProcessSearchPathRead(&before) == UMI_STATUS_OK);
    char directory[UMI_PATH_CAPACITY];
    CHECK(umi_fs_current_directory(directory, sizeof directory) == UMI_STATUS_OK);
    const char *definitions = "UMICOM_RUN_VALUE='caf\xc3\xa9 with spaces; literal $HOME'";
    const char *expected = "received:[caf\xc3\xa9 with spaces; literal $HOME]";
    if (strcmp(argv[1], "run") == 0)
    {
        UmiBuildRunnerConfig config = {0};
        umi_build_profile_init(&config.profile);
        CHECK(strlen(argv[2]) < sizeof config.profile.run_program);
        strcpy(config.profile.run_program, argv[2]);
        strcpy(config.profile.source_directory, directory);
        strcpy(config.profile.run_environment, definitions);
        UmiBuildRunner *runner = NULL;
        CHECK(umi_build_runner_create(&config, &runner) == UMI_STATUS_OK);
        UmiBuildResult *result = NULL;
        CHECK(umi_build_result_create(&result) == UMI_STATUS_OK);
        CHECK(umi_build_runner_run(runner, UMI_BUILD_PHASE_RUN, result) == UMI_STATUS_OK);
        CHECK(result->exit_code == 0 && strstr(result->output, expected) != NULL);
        CHECK(strstr(result->command, "UMICOM_RUN_VALUE") == NULL);
        umi_build_result_destroy(result);
        umi_build_runner_destroy(runner);
    }
    else
    {
        CHECK(strcmp(argv[1], "stream") == 0 || strcmp(argv[1], "empty-value") == 0);
        if (strcmp(argv[1], "empty-value") == 0)
        {
            definitions = "UMICOM_RUN_VALUE=";
            expected = "received:[]";
        }
        UmiLanguageRuntimeProcessStreamConfig config = {0};
        config.program = argv[2];
        config.working_directory = directory;
        UmiLanguageRuntimeProcessStream *stream = NULL;
        CHECK(UmiLanguageRuntimeProcessStreamStartWithEnvironment(&config, NULL, definitions,
                                                                  &stream) == UMI_STATUS_OK);
        char output[4096] = "";
        size_t used = 0U;
        for (unsigned attempt = 0U; attempt < 100U && strchr(output, '\n') == NULL; ++attempt)
        {
            size_t count = 0U;
            UmiStatus status = umi_language_runtime_process_stream_read(
                stream, output + used, sizeof output - used - 1U, 100U, &count);
            CHECK(status == UMI_STATUS_OK || status == UMI_STATUS_NOT_FOUND);
            used += count;
            output[used] = '\0';
            CHECK(used < sizeof output - 1U);
        }
        CHECK(strstr(output, expected) != NULL);
        umi_language_runtime_process_stream_destroy(stream);
    }
    CHECK(UmiProcessSearchPathRead(&after) == UMI_STATUS_OK);
    CHECK(strcmp(before, after) == 0);
    UmiProcessSearchPathFree(before);
    UmiProcessSearchPathFree(after);
    return 0;
}
