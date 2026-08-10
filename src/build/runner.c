/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/runner.c
 *
 * PURPOSE:
 *   Implement synchronous build orchestration with provider selection, output capture, diagnostic parsing and history.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/runner.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/build/cmake_provider.h"
#include "umicom/build/ctest_provider.h"
#include "umicom/build/parser.h"
#include "umicom/platform/process.h"
#include "umicom/platform/threading.h"

struct UmiBuildRunner {
    UmiBuildProfile profile;
    UmiBuildHistory *history;
    UmiClock *clock;
    UmiCancellationToken *cancellation;
    UmiMutex *mutex;
    uint64_t next_operation_id;
};

static UmiBuildProvider provider_for_phase(UmiBuildPhase phase)
{
    if (phase == UMI_BUILD_PHASE_TEST) {
        return umi_build_ctest_provider();
    }
    return umi_build_cmake_provider();
}

UmiStatus umi_build_runner_create(const UmiBuildRunnerConfig *config,
                                  UmiBuildRunner **out_runner)
{
    UmiBuildRunner *runner;
    char message[256];
    UmiStatus status;
    if (config == NULL || out_runner == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_runner = NULL;
    status = umi_build_profile_validate(&config->profile,
                                        message,
                                        sizeof(message));
    if (status != UMI_STATUS_OK) {
        return status;
    }
    runner = (UmiBuildRunner *)calloc(1U, sizeof(*runner));
    if (runner == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    runner->profile = config->profile;
    runner->history = config->history;
    runner->clock = config->clock;
    runner->cancellation = config->cancellation;
    runner->next_operation_id = 1U;
    if (umi_mutex_create(&runner->mutex) != UMI_STATUS_OK) {
        free(runner);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    *out_runner = runner;
    return UMI_STATUS_OK;
}

void umi_build_runner_destroy(UmiBuildRunner *runner)
{
    if (runner == NULL) {
        return;
    }
    umi_mutex_destroy(runner->mutex);
    free(runner);
}

UmiStatus umi_build_runner_set_profile(UmiBuildRunner *runner,
                                       const UmiBuildProfile *profile)
{
    char message[256];
    UmiStatus status;
    if (runner == NULL || profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_build_profile_validate(profile, message, sizeof(message));
    if (status != UMI_STATUS_OK) {
        return status;
    }
    (void)umi_mutex_lock(runner->mutex);
    runner->profile = *profile;
    (void)umi_mutex_unlock(runner->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_build_runner_run(UmiBuildRunner *runner,
                               UmiBuildPhase phase,
                               UmiBuildResult *out_result)
{
    UmiBuildProvider provider;
    UmiBuildCommand command;
    UmiBuildProfile profile;
    UmiProcessRequest request;
    UmiProcessResult process_result;
    uint64_t operation_id;
    uint64_t start_ns = 0U;
    UmiStatus status;

    if (runner == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)umi_mutex_lock(runner->mutex);
    profile = runner->profile;
    operation_id = runner->next_operation_id++;
    (void)umi_mutex_unlock(runner->mutex);

    umi_build_result_init(out_result,
                          operation_id,
                          phase,
                          profile.profile_id);
    out_result->state = UMI_BUILD_STATE_RUNNING;
    provider = provider_for_phase(phase);
    status = umi_build_provider_create_command(&provider,
                                               &profile,
                                               phase,
                                               &command);
    if (status != UMI_STATUS_OK) {
        umi_build_result_finish(out_result, status, -1, 0U);
        return status;
    }
    (void)umi_build_command_format(&command,
                                   out_result->command,
                                   sizeof(out_result->command));

    (void)memset(&request, 0, sizeof(request));
    request.program = command.program;
    request.arguments = command.arguments;
    request.argument_count = command.argument_count;
    request.working_directory =
        command.working_directory[0] != '\0'
            ? command.working_directory
            : NULL;
    request.capture_stdout = 1;
    request.capture_stderr = 1;
    request.timeout_ms = profile.timeout_ms;
    request.cancellation = runner->cancellation;

    if (runner->clock != NULL && runner->clock->monotonic_nanoseconds != NULL) {
        start_ns = runner->clock->monotonic_nanoseconds(runner->clock);
    }
    status = umi_process_execute(&request, &process_result);
    (void)snprintf(out_result->output,
                   sizeof(out_result->output),
                   "%s",
                   process_result.output);
    (void)umi_build_parse_output(out_result->output,
                                 &out_result->diagnostics);
    out_result->started_ns = start_ns;
    umi_build_result_finish(out_result,
                            status,
                            process_result.exit_code,
                            process_result.duration_ms);
    if (runner->history != NULL) {
        (void)umi_build_history_append(runner->history, out_result);
    }
    return status;
}

uint64_t umi_build_runner_next_operation_id(const UmiBuildRunner *runner)
{
    uint64_t value;
    if (runner == NULL) {
        return 0U;
    }
    (void)umi_mutex_lock(runner->mutex);
    value = runner->next_operation_id;
    (void)umi_mutex_unlock(runner->mutex);
    return value;
}
